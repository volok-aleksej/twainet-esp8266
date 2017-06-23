#include "remote_logger.h"
#include "vector.h"

template<> const ProtobufCMessageDescriptor& RemoteLogMessage::descriptor = remote_log__log__descriptor;

RemoteLogger::RemoteLogger()
{
}

RemoteLogger::~RemoteLogger()
{
}

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
    GetTwainetClient()->toMessage(logMsg, modules[0]);
    return true;
}
