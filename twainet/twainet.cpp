#include "twainet.h"
#include "thread_manager.h"
#include "console.h"
#include "ssl_crypto.h"

static const int commandSize = 1024;
static char command[commandSize];

extern "C" void twainetAppRun(void (*main)(void))
{
    RNG_initialize();
    Console console;
    SetConsole(&console);
    while(true) {
        main();
        console.Read(command, commandSize);
        ManagersContainer::GetInstance().CheckManagers();
        ThreadManager::GetInstance().SwitchThread();
    }
    RNG_terminate();
}