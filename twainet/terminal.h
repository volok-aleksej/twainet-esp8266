#ifndef REMOTE_LOOGER_H
#define REMOTE_LOOGER_H

#include "console.h"
#include "twainet.h"
#include "terminal.pb-c.h"

class Terminal;

typedef ProtoMessage<Terminal__TermName, Terminal> TermNameMessage;
typedef ProtoMessage<Terminal__Log, Terminal> LogMessage;
typedef ProtoMessage<Terminal__Command, Terminal> CommandMessage;
typedef ProtoMessage<Terminal__GetNextCommandArgs, Terminal> GetNextCommandArgsMessage;
typedef ProtoMessage<Terminal__NextCommandArgs, Terminal> NextCommandArgsMessage;

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
    void onMessage(const Terminal__NextCommandArgs& msg){}
    void onMessage(const Terminal__Command& msg);
    void onMessage(const Terminal__GetNextCommandArgs& msg);
private:
    twnstd::vector<DataMessage*> m_messages;
};

#endif/*REMOTE_LOOGER_H*/
