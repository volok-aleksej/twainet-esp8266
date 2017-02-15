#include "twainet.h"
#include "thread_manager.h"
#include "ssl/ssl_crypto.h"

extern "C" void twainetAppRun(void (*main)(void))
{
    RNG_initialize();
    while(true) {
        main();
        ManagersContainer::GetInstance().CheckManagers();
        ThreadManager::GetInstance().SwitchThread();
    }
    RNG_terminate();
}