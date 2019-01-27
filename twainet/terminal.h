#ifndef REMOTE_LOOGER_H
#define REMOTE_LOOGER_H

#include "console.h"
#include "twainet.h"
#include "terminal.pb-c.h"

class Terminal;

typedef ProtoMessage<Terminal__TermName, Terminal> TermNameMessage;
typedef ProtoMessage<Terminal__Log, Terminal> LogMessage;
typedef ProtoMessage<Terminal__Command, Terminal> CommandMessage;
typedef ProtoMessage<Terminal__GetCommandList, Terminal> GetCommandListMessage;
typedef ProtoMessage<Terminal__CommandList, Terminal> CommandListMessage;

class Terminal : public Console
{
public:
    Terminal();
    virtual ~Terminal();
    
    bool Write(const char* log) override;

    void onConnected();
    bool onData(const String& messageName, const char* data, int len);
    bool toMessage(const DataMessage& msg);
    
protected:
    void addMessage(DataMessage* msg);
    
    template<typename TMessage, typename THandler> friend class ProtoMessage;
    void onMessage(const Terminal__Log& msg){}
    void onMessage(const Terminal__TermName& msg){}
    void onMessage(const Terminal__CommandList& msg){}
    void onMessage(const Terminal__Command& msg);
    void onMessage(const Terminal__GetCommandList& msg);
private:
    twnstd::vector<DataMessage*> m_messages;
};

#endif/*REMOTE_LOOGER_H*/
