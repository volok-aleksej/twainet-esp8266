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
    void DoCommand(const String& command, const twnstd::vector<String>& params);
    void GetCommandList(twnstd::vector<CommandBase*>& commands);
private:
    twnstd::vector<ICommand*> m_commands;
};

#endif/*COMMAND_LINE_H*/
