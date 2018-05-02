#include "twainet.h"
#include "thread_manager.h"
#include "terminal.h"
#include "managers_container.h"
#include "ipc_connector_factory.h"
#include "console.h"
#include "ssl_crypto.h"
#include "logger.h"

static const int commandSize = 1024;
static char command[commandSize];

extern void mainloop();
extern void usersetup();

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

extern "C" void setup()
{
    CommandLine::GetInstance().AddCommand(CreateCommand("ipcinfo", &ipcInfo));
    usersetup();
}

extern "C" void loop()
{
    RNG_initialize();
    Terminal console;
    console.Init();
    SetConsole(&console);
    while(true) {
        mainloop();
        if(console.Read(command, commandSize)) {
            CommandLine::GetInstance().DoCommand(command, strlen(command));
        }
        ManagersContainer::GetInstance().CheckManagers();
        ThreadManager::GetInstance().SwitchThread();
    }
    RNG_terminate();
}
