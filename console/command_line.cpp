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
    twnstd::vector<String> commandlist;
    String command = (args.length() == 0) ? "" : const_cast<twnstd::vector<String>&>(args)[0];
    for(int i = 0; i < m_commands.length(); i++) {
        if(m_commands[i]->IsCommand(command)) {
            const_cast<twnstd::vector<String>&>(args).erase(0);
            return m_commands[i]->getNextCommandArgs(args, new_word);
        }
        commandlist.push_back(m_commands[i]->toCommandBase()->m_command);
    }
    if(args.length() <= 1) {
        return commandlist;
    }
    return twnstd::vector<String>();
}

