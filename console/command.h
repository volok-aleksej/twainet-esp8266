#ifndef COMMAND_H
#define COMMAND_H

#include <WString.h>
#include <vector.h>

class CommandBase;

class ICommand
{
public:
    virtual ~ICommand(){}
    virtual bool IsCommand(const String& command) = 0;
    virtual void doCommand(const String& params) = 0;
    virtual CommandBase* toCommandBase() = 0;
};

class CommandBase
{
public:
    CommandBase(){}
    CommandBase(const String& command, const twnstd::vector<String>& args = twnstd::vector<String>())
    : m_command(command), m_args(args){}
    CommandBase(const CommandBase& cmd)
    : m_command(cmd.m_command), m_args(cmd.m_args){}
    virtual ~CommandBase(){}
    
    void operator = (const CommandBase& cmd)
    {
        m_command = cmd.m_command;
        m_args = cmd.m_args;
    }
    
    String m_command;
    twnstd::vector<String> m_args;
};

template <typename Func>
class Command : public ICommand, public CommandBase
{
public:
    Command(Func func, const String& command, const twnstd::vector<String>& args = twnstd::vector<String>())
        : CommandBase(command, args), m_func(func){}
    virtual ~Command(){}
    
    bool IsCommand(const String& command) override
    {
        return m_command == command;
    }
    
    void doCommand(const String& params) override
    {
        m_func(params);
    }
    
    CommandBase* toCommandBase() override
    {
        return static_cast<CommandBase*>(this);
    }
    
private:
    Func m_func;
};

template<typename Func>
Command<Func>* CreateCommand(Func func, const String& command, const twnstd::vector<String>& args = twnstd::vector<String>()) {
    return new Command<Func>(func, command, args);
}

#endif/*COMMAND_H*/