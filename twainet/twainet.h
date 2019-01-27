#ifndef TWAINET_H
#define TWAINET_H

#include "ipc_module.h"
#include "twainet_module.h"
#include "command_line.h"
#include "config.h"

TwainetModule* GetTwainetClient();
Config*        GetConfig();

#endif/*TWAINET_H*/
