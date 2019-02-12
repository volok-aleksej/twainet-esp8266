#include "terminal.h"
#include "vector.h"

char nameKey[] = "name";

template<> const ProtobufCMessageDescriptor& LogMessage::descriptor = terminal__log__descriptor;
template<> const ProtobufCMessageDescriptor& CommandMessage::descriptor = terminal__command__descriptor;
template<> const ProtobufCMessageDescriptor& TermNameMessage::descriptor = terminal__term_name__descriptor;
template<> const ProtobufCMessageDescriptor& GetNextCommandArgsMessage::descriptor = terminal__get_next_command_args__descriptor;
template<> const ProtobufCMessageDescriptor& NextCommandArgsMessage::descriptor = terminal__next_command_args__descriptor;

Terminal::Terminal()
{
    GetTwainetClient()->SetTerminal(this);
    addMessage(new CommandMessage(this));
    addMessage(new GetNextCommandArgsMessage(this));
    GetConfig()->addKey(nameKey);
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
    String terminal_name = GetConfig()->getValue(nameKey);
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

void Terminal::onMessage(const Terminal__GetNextCommandArgs& msg)
{
    twnstd::vector<String> args;
    for(int i = 0; i < const_cast<Terminal__GetNextCommandArgs&>(msg).n_args; i++) {
        args.push_back(const_cast<Terminal__GetNextCommandArgs&>(msg).args[i]);
    }
    bool new_word = false;
    twnstd::vector<String> commands = CommandLine::GetInstance().GetNextCommandArgs(args, new_word);

    NextCommandArgsMessage ncaMsg;
    ncaMsg.GetMessage()->new_word = new_word ? 1 : 0;
    ncaMsg.GetMessage()->n_args = commands.length();
    ncaMsg.GetMessage()->args = (char**)(malloc(commands.length()*sizeof(char*)));
    for(int i = 0; i < commands.length(); i++) {
        ncaMsg.GetMessage()->args[i] = (char*)commands[i].c_str();
    }

    toMessage(ncaMsg);

    free(ncaMsg.GetMessage()->args);
}
