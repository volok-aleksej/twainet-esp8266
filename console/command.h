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
    virtual void doCommand(const twnstd::vector<String>& params) = 0;
    virtual twnstd::vector<String> getNextCommandArgs(const twnstd::vector<String>& params, bool& new_word) = 0;
    virtual CommandBase* toCommandBase() = 0;
};

class CommandBase
{
public:
    CommandBase(){}
    CommandBase(const char* command)
    : m_command(command){}
    CommandBase(const CommandBase& cmd)
    : m_command(cmd.m_command){}
    virtual ~CommandBase(){}
    
    void operator = (const CommandBase& cmd)
    {
        m_command = cmd.m_command;
    }
    
    const char* m_command;
};

template <typename Func>
class Command : public ICommand, public CommandBase
{
public:
    Command(Func func, const char* command)
        : CommandBase(command), m_func(func){}
    virtual ~Command(){}
    
    bool IsCommand(const String& command) override
    {
        return command == m_command;
    }
    
    void doCommand(const twnstd::vector<String>& params) override
    {
        m_func->doCommand(params);
    }
    
    twnstd::vector<String> getNextCommandArgs(const twnstd::vector<String>& params, bool& new_word) override
    {
        return m_func->getNextCommandArgs(params, new_word);
    }

    CommandBase* toCommandBase() override
    {
        return static_cast<CommandBase*>(this);
    }
    
private:
    Func m_func;
};

typedef void (*Func)(const twnstd::vector<String>&);
template <> class Command<Func> : public ICommand, public CommandBase
{
public:
    Command(Func func, const char* command)
        : CommandBase(command), m_func(func){}
    virtual ~Command(){}

    bool IsCommand(const String& command) override
    {
        return command == m_command;
    }

    void doCommand(const twnstd::vector<String>& params) override
    {
        m_func(params);
    }

    twnstd::vector<String> getNextCommandArgs(const twnstd::vector<String>& params, bool& new_word) override
    {
        return twnstd::vector<String>();
    }

    CommandBase* toCommandBase() override
    {
        return static_cast<CommandBase*>(this);
    }

private:
    Func m_func;
};

template<typename Func>
Command<Func>* CreateCommand(Func func, const char* command) {
    return new Command<Func>(func, command);
}

#endif/*COMMAND_H*/
