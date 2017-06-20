#ifndef COMMAND_LINE_H
#define COMMAND_LINE_H

#include "vector.h"
#include "command.h"
#include "singleton.h"

class CommandLine : public Singleton<CommandLine>
{
protected:
    friend class Singleton<CommandLine>;
    CommandLine();
    ~CommandLine();
public:
    void AddCommand(ICommand* command);
    void DoCommand(char* command, int len);
private:
    twnstd::vector<ICommand*> m_commands;
};

#endif/*COMMAND_LINE_H*/