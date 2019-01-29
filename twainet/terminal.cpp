#include "terminal.h"
#include "vector.h"

template<> const ProtobufCMessageDescriptor& LogMessage::descriptor = terminal__log__descriptor;
template<> const ProtobufCMessageDescriptor& CommandMessage::descriptor = terminal__command__descriptor;
template<> const ProtobufCMessageDescriptor& TermNameMessage::descriptor = terminal__term_name__descriptor;
template<> const ProtobufCMessageDescriptor& GetCommandListMessage::descriptor = terminal__get_command_list__descriptor;
template<> const ProtobufCMessageDescriptor& CommandListMessage::descriptor = terminal__command_list__descriptor;

Terminal::Terminal()
{
    GetTwainetClient()->SetTerminal(this);
    addMessage(new CommandMessage(this));
    addMessage(new GetCommandListMessage(this));
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
    logMsg.GetMessage()->data = (char*)log;
    logMsg.GetMessage()->time = millis();
    toMessage(logMsg);
    return true;
}

void Terminal::onConnected()
{
    TermNameMessage msg;
    String terminal_name = GetConfig()->getValue("name");
    msg.GetMessage()->name = (char*)terminal_name.c_str();
    toMessage(msg);
}

void Terminal::addMessage(DataMessage* msg)
{
    m_messages.push_back(msg);
}

bool Terminal::toMessage(const DataMessage& msg)
{
    String sessionId = GetTwainetClient()->GetSessionId();
    if(!sessionId.length()) {
        return false;
    }
    
    IPCObjectName name(ClientModule::m_serverIPCName, sessionId);
    GetTwainetClient()->toMessage(msg, name);
}

bool Terminal::onData(const String& messageName, const char* data, int len)
{
    if(len < 0) {
        return false;
    }
    for (unsigned int i = 0; i < m_messages.length(); ++i)
    {
        if(strcmp(messageName.c_str(),m_messages[i]->GetName()) == 0)
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
    String command = msg.cmd;
    for(int i = 0; i < msg.n_args; i++) {
        command += " ";
        command += msg.args[i];
    }
    CommandLine::GetInstance().DoCommand((char*)command.c_str(), command.length());
}

void Terminal::onMessage(const Terminal__GetCommandList& msg)
{
    twnstd::vector<CommandBase*> commands;
    CommandLine::GetInstance().GetCommandList(commands);
    CommandListMessage clMsg;
    clMsg.GetMessage()->n_cmd = commands.length();
    clMsg.GetMessage()->cmd = (Terminal__Command**)(malloc(commands.length()*(sizeof(Terminal__Command) + sizeof(Terminal__Command*))));
    for(int i = 0; i < commands.length(); i++) {
        clMsg.GetMessage()->cmd[i] = (Terminal__Command*)(((char*)clMsg.GetMessage()->cmd) + 
                                         sizeof(Terminal__Command*)*commands.length() + i*sizeof(Terminal__Command));
        clMsg.GetMessage()->cmd[i]->cmd = (char*)commands[i]->m_command.c_str();
        clMsg.GetMessage()->cmd[i]->n_args = commands[i]->m_args.length();
        clMsg.GetMessage()->cmd[i]->args = (char**)malloc(commands[i]->m_args.length());
        for(int j = 0; j < commands[i]->m_args.length(); j++) {
            clMsg.GetMessage()->cmd[i]->args[j] = (char*)commands[i]->m_args[j].c_str();
        }
    }
    
    LOG_INFO("send answer message %s", clMsg.GetMessageName());
    toMessage(clMsg);

    for(int i = 0; i < commands.length(); i++) {
        free(clMsg.GetMessage()->cmd[i]->args);
    }
    free(clMsg.GetMessage()->cmd);
}
