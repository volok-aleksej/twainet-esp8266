#include "twainet.h"
#include "thread_manager.h"
#include "terminal.h"
#include "managers_container.h"
#include "ipc_connector_factory.h"
#include "console.h"
#include "ssl_crypto.h"
#include "logger.h"
#include "ESP8266WiFi.h"
#include "utils.h"
#include "FS.h"

static const int commandSize = 1024;
static char command[commandSize];

extern void mainloop();
extern void usersetup();

Config g_config;

char wssidKey[] = "wifi.ssid";
char wpassKey[] = "wifi.pass";
char suserKey[] = "server.user";
char spassKey[] = "server.pass";
char sipKey[] = "server.ip";
char sportKey[] = "server.port";
char nameKey[] = "name";
char commandSet[] = "set";
char commandGet[] = "get";
char commandWrite[] = "write";
char warn_abs_key[] = "absent key name in command";
char warn_abs_value[] = "absent value in command";

TwainetModule* GetTwainetClient()
{
  static IPCObjectName moduleName("");
  static IPCConnectorFactory<ClientConnector> factory(moduleName);
  static TwainetModule module(moduleName, &factory);
  return &module;
}

Config* GetConfig()
{
    return &g_config;
}

void ipcInfo(const twnstd::vector<String>& params)
{
    twnstd::vector<IPCObjectName> objects = GetTwainetClient()->GetIPCObjects();
    LOG_INFO("ipc objects size %d:", objects.length());
    for(int i = 0; i< objects.length(); i++) {
        LOG_INFO("%s", objects[i].GetModuleNameString().c_str());
    }
}

struct ConfigCommand
{
    void doCommand(const twnstd::vector<String>& params)
    {
        if(!params.length()) {
            LOG_WARNING("use %s|%s|%s", commandSet, commandGet, commandWrite);
        } else if(params.length() == 1 &&
            (const_cast<twnstd::vector<String>&>(params)[0] == commandSet || const_cast<twnstd::vector<String>&>(params)[0] == commandGet)) {
            LOG_WARNING("absent key in command");
        } else if(params.length() == 2 && const_cast<twnstd::vector<String>&>(params)[0] == commandSet) {
            LOG_WARNING("absent value in command");
        }
        if(const_cast<twnstd::vector<String>&>(params)[0] == commandSet) {
            g_config.setValue(const_cast<twnstd::vector<String>&>(params)[1], const_cast<twnstd::vector<String>&>(params)[2]);
        } else if(const_cast<twnstd::vector<String>&>(params)[0] == commandGet) {
            LOG_INFO("%s", g_config.getValue(const_cast<twnstd::vector<String>&>(params)[1]).c_str());
        } else if(const_cast<twnstd::vector<String>&>(params)[0] == commandWrite) {
            g_config.Write();
        }
    }

    twnstd::vector<String> getNextCommandArgs(const twnstd::vector<String>& params)
    {
        if(!params.length()) {
            twnstd::vector<String> args;
            args.push_back(commandSet);
            args.push_back(commandGet);
            args.push_back(commandWrite);
            return args;
        } else if(params.length() == 1 && (const_cast<twnstd::vector<String>&>(params)[0] == commandSet ||
                                            const_cast<twnstd::vector<String>&>(params)[0] == commandGet)) {
            twnstd::vector<String> args;
            args.push_back(wssidKey);
            args.push_back(wpassKey);
            args.push_back(suserKey);
            args.push_back(spassKey);
            args.push_back(sipKey);
            args.push_back(sportKey);
            args.push_back(nameKey);
        } else {
            return twnstd::vector<String>();
        }
    }
};

ConfigCommand config;

extern "C" void setup()
{
    Serial.begin(115200);
    while (!Serial);
    
    SPIFFS.begin();
    g_config.Read();
    
    // check for the presence of the shield:
    while (WiFi.status() == WL_NO_SHIELD) {}
    
    String ssid = g_config.getValue(wssidKey);
    String pass = g_config.getValue(wpassKey);
    if(ssid.length()) {
        WiFi.mode(WIFI_STA);
        WiFi.begin(ssid.c_str(), pass.c_str());
    }
    
    CommandLine::GetInstance().AddCommand(CreateCommand(&ipcInfo, "ipcinfo"));
    CommandLine::GetInstance().AddCommand(CreateCommand(&config, "config"));
    usersetup();
}

extern "C" void loop()
{
    RNG_initialize();
    Terminal console;
    console.Init();
    SetConsole(&console);
    bool status = false;
    while(true) {
        mainloop();
        
        unsigned int stat = WiFi.status();
        if(stat == WL_CONNECTED && !status) {
            LOG_INFO("wifi connected");
            String userName = g_config.getValue(suserKey);
            String password = g_config.getValue(spassKey);
            String ip = g_config.getValue(sipKey);
            String port = g_config.getValue(sportKey);
            GetTwainetClient()->SetUserName(userName);
            GetTwainetClient()->SetPassword(password);
            GetTwainetClient()->Connect(ip, port.toInt());
            status = true;
        } else if(stat == WL_DISCONNECTED && status){
            LOG_INFO("wifi disconnected");
            GetTwainetClient()->Disconnect();
            status = false;
        }
        delay(150);
        
        if(console.Read(command, commandSize)) {
            String commandStr;
            twnstd::vector<String> params = getSubstrings(command, " ");
            if(params.length()) {
                commandStr = params[0];
                params.erase(0);
                CommandLine::GetInstance().DoCommand(commandStr, params);
            }
        }
        ManagersContainer::GetInstance().CheckManagers();
        ThreadManager::GetInstance().SwitchThread();
    }
    RNG_terminate();
}
