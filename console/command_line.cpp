#include "command_line.h"
#include <WString.h>

CommandLine::CommandLine()
{

}

CommandLine::~CommandLine()
{
    for(int i = 0; i < m_commands.length(); i++) {
        delete m_commands[i];
    }
}

void CommandLine::AddCommand(ICommand* command)
{
    m_commands.push_back(command);
}

void CommandLine::DoCommand(const String& command, const twnstd::vector<String>& params)
{
    for(int i = 0; i < m_commands.length(); i++) {
        if(m_commands[i]->IsCommand(command)) {
            m_commands[i]->doCommand(params);
        }
    }
}

twnstd::vector<String> CommandLine::GetNextCommandArgs(const twnstd::vector<String>& args, bool& new_word)
{
    twnstd::vector<String> args_= args;
    twnstd::vector<String> commandlist;
    String command = (args_.length() == 0) ? "" : const_cast<twnstd::vector<String>&>(args_)[0];
    for(int i = 0; i < m_commands.length(); i++) {
        if(m_commands[i]->IsCommand(command)) {
            const_cast<twnstd::vector<String>&>(args_).erase(0);
            return m_commands[i]->getNextCommandArgs(args_, new_word);
        }
        commandlist.push_back(m_commands[i]->toCommandBase()->m_command);
    }
    new_word = (args_.length() == 0);
    if(args_.length() <= 1) {
        return commandlist;
    }
    return twnstd::vector<String>();
}

