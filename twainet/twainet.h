#ifndef TWAINET_H
#define TWAINET_H

#include "ipc_module.h"
#include "twainet_module.h"
#include "command_line.h"
#include "twn_config.h"

extern char wssidKey[];
extern char wpassKey[];
extern char suserKey[];
extern char spassKey[];
extern char sipKey[];
extern char sportKey[];
extern char nameKey[];

TwainetModule* GetTwainetClient();
Config*        GetConfig();

#endif/*TWAINET_H*/
