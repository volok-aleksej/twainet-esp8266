#ifndef CLIENT_SIGNAL_HANDLER_H
#define CLIENT_SIGNAL_HANDLER_H

#include "connector_messages.h"
#include "client_connector.h"

class ClientModule;

class ClientSignalHandler : public SignalReceiver
{
public:
	ClientSignalHandler(ClientModule* module);
	~ClientSignalHandler();
	
	void onAddClientServerConnector(const ConnectorMessage& msg);
	void onErrorConnect(const ConnectErrorMessage& msg);
	void onConnected(const ClientServerConnectedMessage& msg);

	void onLoginResult(const LoginResultMessage& msg);
private:
	ClientModule* m_module;
};

#endif/*CLIENT_SIGNAL_HANDLER_H*/