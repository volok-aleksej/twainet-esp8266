#include "twainet.h"
#include "thread_manager.h"
#include "terminal.h"
#include "managers_container.h"
#include "ipc_connector_factory.h"
#include "console.h"
#include "ssl_crypto.h"
#include "logger.h"
#include "ESP8266WiFi.h"
#include "config.h"
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
char commandSet[] = "set";
char commandGet[] = "get";
char commandWrite[] = "write";

TwainetModule* GetTwainetClient()
{
  static IPCObjectName moduleName("");
  static IPCConnectorFactory<ClientConnector> factory(moduleName);
  static TwainetModule module(moduleName, &factory);
  return &module;
}

void ipcInfo(const String& params)
{
    twnstd::vector<IPCObjectName> objects = GetTwainetClient()->GetIPCObjects();
    LOG_INFO("ipc objects size %d:", objects.length());
    for(int i = 0; i< objects.length(); i++) {
        LOG_INFO("%s", objects[i].GetModuleNameString().c_str());
    }
}

void config(const String& params)
{
    twnstd::vector<String> params_ = getSubstrings(params, " ");
    twnstd::vector<String> keys;
    if(!params_.length()) {
        LOG_WARNING("use %s|%s|%s", commandSet, commandGet, commandWrite);
    } else if(params_.length() == 1 && 
       (params_[0] == commandSet || params_[0] == commandGet)) {
        LOG_WARNING("absent key in command");
    } else if(params_.length() == 2 && params_[0] == commandSet) {
        LOG_WARNING("absent value in command");
    }
    if(params_[0] == commandSet) {
        g_config.setValue(params_[1], params_[2]);
    } else if(params_[0] == commandGet) {
        LOG_INFO("%s", g_config.getValue(params_[1]).c_str());
    } else if(params_[0] == commandWrite) {
        g_config.Write();
    }
}

extern "C" void setup()
{
    Serial.begin(115200);
    while (!Serial);
    
    SPIFFS.begin();
    g_config.Read();
    
    // check for the presence of the shield:
    if (WiFi.status() == WL_NO_SHIELD) {
        while (true);
    }
    
    String ssid = g_config.getValue(wssidKey);
    String pass = g_config.getValue(wpassKey);
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid.c_str(), pass.c_str());
    
    CommandLine::GetInstance().AddCommand(CreateCommand("ipcinfo", &ipcInfo));
    CommandLine::GetInstance().AddCommand(CreateCommand("config", &config));
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
            CommandLine::GetInstance().DoCommand(command, strlen(command));
        }
        ManagersContainer::GetInstance().CheckManagers();
        ThreadManager::GetInstance().SwitchThread();
    }
    RNG_terminate();
}
