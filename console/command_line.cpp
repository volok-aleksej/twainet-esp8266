#include "command_line.h"

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

void CommandLine::DoCommand(char* command, int len)
{
    String commandStr, params;
    for(int i = 0; i < len; i++) {
        if(command[i] == ' ') {
            params = command + i + 1;
            break;
        }
        commandStr.concat(command[i]);
    }
    for(int i = 0; i < m_commands.length(); i++) {
        if(m_commands[i]->IsCommand(commandStr)) {
            m_commands[i]->doCommand(params);
        }
    }
}
