#ifndef CONNECTOR_MESSAGES_H
#define CONNECTOR_MESSAGES_H

#include <WString.h>

#include "data_message.h"

/******************************************************************/

class Connector;

class ConnectorMessage : public DataMessage
{
public:
	ConnectorMessage(Connector* conn);
    virtual char* GetName()const;
	static const char* GetMessageName();
public:
	Connector* m_conn;
};

/******************************************************************/

class DisconnectedMessage : public DataMessage
{
public:
	DisconnectedMessage(const String& id, const String& connId);
    virtual char* GetName()const;
	static const char* GetMessageName();
public:
	String m_id;
	String m_connId;
};

/******************************************************************/

class ConnectedMessage : public DataMessage
{
public:
	ConnectedMessage(const String& id);
    virtual char* GetName()const;
	static const char* GetMessageName();
public:
	String m_id;
};

/******************************************************************/

class ErrorMessage : public DataMessage
{
public:
	ErrorMessage(const String& error, int errorCode);
    virtual char* GetName()const;
	static const char* GetMessageName();
public:
	String m_error;
	int m_errorCode;
};

/******************************************************************/

class ConnectErrorMessage : public ErrorMessage
{
public:
        ConnectErrorMessage(const String& moduleName, const String& error, int errorCode);
        virtual char* GetName()const;
        static const char* GetMessageName();
public:
        String m_moduleName;
};

/******************************************************************/

class ClientServerConnectedMessage : public ConnectedMessage
{
public:
        ClientServerConnectedMessage(const String& id);
        virtual char* GetName()const;
        static const char* GetMessageName();
};

/******************************************************************/

#endif/*CONNECTOR_MESSAGES_H*/