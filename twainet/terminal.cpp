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
    twnstd::vector<String> args;
    for(int i = 0; i < msg.n_args; i++) {
        args.push_back(msg.args[i]);
    }
    CommandLine::GetInstance().DoCommand(command, args);
}

void Terminal::onMessage(const Terminal__GetCommandList& msg)
{
    twnstd::vector<CommandBase*> commands;
    CommandLine::GetInstance().GetCommandList(commands);

    twnstd::vector<CommandMessage> msgcommands;
    msgcommands.resize(commands.length());

    CommandListMessage clMsg;
    clMsg.GetMessage()->n_cmd = commands.length();
    clMsg.GetMessage()->cmd = (Terminal__Command**)(malloc(commands.length()*sizeof(Terminal__Command*)));
    for(int i = 0; i < commands.length(); i++) {
        msgcommands.push_back(CommandMessage());
        clMsg.GetMessage()->cmd[i] = msgcommands[i].GetMessage();
        clMsg.GetMessage()->cmd[i]->cmd = (char*)commands[i]->m_command.c_str();
        clMsg.GetMessage()->cmd[i]->n_args = commands[i]->m_args.length();
        if(commands[i]->m_args.length()) {
            clMsg.GetMessage()->cmd[i]->args = (char**)malloc(commands[i]->m_args.length()*sizeof(char*));
        } else {
            clMsg.GetMessage()->cmd[i]->args = 0;
        }
        for(int j = 0; j < commands[i]->m_args.length(); j++) {
            clMsg.GetMessage()->cmd[i]->args[j] = (char*)commands[i]->m_args[j].c_str();
        }
    }
    
    toMessage(clMsg);

    for(int i = 0; i < commands.length(); i++) {
        if(clMsg.GetMessage()->cmd[i]->args)
            free(clMsg.GetMessage()->cmd[i]->args);
    }
    free(clMsg.GetMessage()->cmd);
}
