#ifndef REMOTE_LOOGER_H
#define REMOTE_LOOGER_H

#include "console.h"
#include "twainet.h"
#include "terminal.pb-c.h"

class Terminal;

typedef ProtoMessage<Terminal__Log, Terminal> LogMessage;
typedef ProtoMessage<Terminal__Command, Terminal> CommandMessage;

class Terminal : public Console
{
public:
    Terminal();
    virtual ~Terminal();
    
    bool Write(const char* log) override;

    void addMessage(DataMessage* msg);
    bool onData(const String& messageName, const char* data, int len);
    bool toMessage(const DataMessage& msg);
    
    void onMessage(const Terminal__Log& msg){}
    void onMessage(const Terminal__Command& msg);
private:
    twnstd::vector<DataMessage*> m_messages;
};

#endif/*REMOTE_LOOGER_H*/
