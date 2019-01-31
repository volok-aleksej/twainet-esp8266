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

void CommandLine::GetCommandList(twnstd::vector<CommandBase*>& commands)
{
    for(int i = 0; i < m_commands.length(); i++) {
        commands.push_back(m_commands[i]->toCommandBase());
    }
}
