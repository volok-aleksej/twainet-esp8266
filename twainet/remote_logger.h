#ifndef REMOTE_LOOGER_H
#define REMOTE_LOOGER_H

#include "console.h"
#include "twainet.h"
#include "remote_log.pb-c.h"

class RemoteLogger;

typedef ProtoMessage<RemoteLog__Log, RemoteLogger> RemoteLogMessage;

class RemoteLogger : public Console
{
public:
    RemoteLogger();
    virtual ~RemoteLogger();
    bool Write(const char* log) override;
    bool Read(char* buf, int bufLen) override;

    void onMessage(const RemoteLog__Log& msg){}
};

#endif/*REMOTE_LOOGER_H*/
