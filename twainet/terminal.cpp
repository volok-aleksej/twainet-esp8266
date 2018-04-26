#include "terminal.h"
#include "vector.h"

template<> const ProtobufCMessageDescriptor& LogMessage::descriptor = terminal__log__descriptor;
template<> const ProtobufCMessageDescriptor& CommandMessage::descriptor = terminal__command__descriptor;

Terminal::Terminal()
{
    GetTwainetClient()->SetTerminal(this);
    addMessage(new CommandMessage(this));
}

Terminal::~Terminal()
{
}

bool Terminal::Write(const char* log)
{
    if(!Console::Write(log)) {
        return false;
    }

    LogMessage logMsg;
    logMsg.GetMessage()->log = (char*)log;
    logMsg.GetMessage()->time = millis();
    toMessage(logMsg);
    return true;
}

void Terminal::addMessage(DataMessage* msg)
{
    m_messages.push_back(msg);
}

bool Terminal::toMessage(const DataMessage& msg)
{
    twnstd::vector<IPCObjectName> modules = GetTwainetClient()->GetConnectedModules();
    if(!modules.length()) {
        return false;
    }

    GetTwainetClient()->toMessage(msg, modules[0]);
}

bool Terminal::onData(const String& messageName, const char* data, int len)
{
    if(len < 0) {
        return false;
    }
    for (unsigned int i = 0; i < m_messages.length(); ++i)
    {
        if(messageName == m_messages[i]->GetName())
        {
            m_messages[i]->serialize((char*)data, len);
            m_messages[i]->onMessage();
            return true;
        }
    }
    return false;
}

void Terminal::onMessage(const Terminal__Command& msg)
{
    CommandLine::GetInstance().DoCommand(msg.command, strlen(msg.command));
}
