#ifndef TWAINET_MODULE_H
#define TWAINET_MODULE_H

#include "client_module.h"

class TwainetModule : public ClientModule
{
public:
	TwainetModule(const IPCObjectName& ipcName, ConnectorFactory* factory);
	virtual ~TwainetModule();
public:
    void toMessage(const DataMessage& message, const IPCObjectName& path);
protected:
	virtual void OnMessage(const String& messageName, const twnstd::vector<String>& path, const char* data, unsigned int lenData);
};


#endif/*TWAINET_MODULE_H*/
