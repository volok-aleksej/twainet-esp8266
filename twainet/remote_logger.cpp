#include "remote_logger.h"
#include "../std/vector.h"

template<> const ProtobufCMessageDescriptor& RemoteLogMessage::descriptor = remote_log__log__descriptor;

RemoteLogger::RemoteLogger()
{
}

RemoteLogger::~RemoteLogger()
{
}

void RemoteLogger::Init()
{
}
/*
bool RemoteLogger::Read(char* buf, int bufLen)
{
    if(Console::Read(buf, bufLen)){
        return true;
    }

    return false;
}

bool RemoteLogger::Write(const char* log)
{
    if(!Console::Write(log)) {
        return false;
    }

    twnstd::vector<IPCObjectName> modules = GetTwainetClient()->GetConnectedModules();
    if(!modules.length()) {
        return false;
    }

    RemoteLogMessage logMsg;
    logMsg.GetMessage()->log = (char*)log;
    logMsg.GetMessage()->time = millis();
    int len = 0;
    logMsg.deserialize(0, len);
    char* data = (char*)malloc(len);
    logMsg.deserialize(data, len);

    Ipc__IPCName path;
    Ipc__IPCName* ppath = &path;
    path.host_name = (char*)modules[0].GetHostName().c_str();
    path.module_name = (char*)modules[0].GetModuleName().c_str();
    path.conn_id = (char*)modules[0].GetConnId().c_str();

    IPCProtoMessage message;
    message.GetMessage()->message_name = (char*)logMsg.GetMessageName();
    message.GetMessage()->ipc_path = &ppath;
    message.GetMessage()->n_ipc_path = 1;
    message.GetMessage()->n_ipc_sender = 0;
    message.GetMessage()->ipc_sender = 0;
    message.GetMessage()->message.data = (uint8_t*)data;
    message.GetMessage()->message.len = len;

    IPCSignalMessage msgSignal(message);
    GetTwainetClient()->SendMsg(msgSignal);

    free(data);
    return true;
}*/
