#include "twainet.h"
#include "thread_manager.h"
#include "terminal.h"
#include "managers_container.h"
#include "ipc_connector_factory.h"
#include "console.h"
#include "ssl_crypto.h"

static const int commandSize = 1024;
static char command[commandSize];

extern void mainloop();

TwainetModule* GetTwainetClient()
{
  static IPCObjectName moduleName("");
  static IPCConnectorFactory<ClientConnector> factory(moduleName);
  static TwainetModule module(moduleName, &factory);
  return &module;
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
