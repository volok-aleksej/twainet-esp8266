#ifndef TWAINET_H
#define TWAINET_H

#include "ipc_module.h"
#include "thread_manager.h"
#include "managers_container.h"
#include "include/ipc_connector_factory.h"

extern "C" void twainetAppRun(void (*main)(void));

#endif/*TWAINET_H*/