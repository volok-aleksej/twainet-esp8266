#include "twainet.h"
#include "thread_manager.h"
#include "remote_logger.h"
#include "ssl_crypto.h"

static const int commandSize = 1024;
static char command[commandSize];

extern void mainloop();

ClientModule* GetTwainetClient()
{
  static IPCObjectName moduleName("");
  static IPCConnectorFactory<ClientConnector> factory(moduleName);
  static ClientModule module(moduleName, &factory);
  return &module;
}

extern "C" void loop()
{
    RNG_initialize();
    RemoteLogger console;
    SetConsole(&console);
    while(true) {
        mainloop();
        console.Read(command, commandSize);
        ManagersContainer::GetInstance().CheckManagers();
        ThreadManager::GetInstance().SwitchThread();
    }
    RNG_terminate();
}
