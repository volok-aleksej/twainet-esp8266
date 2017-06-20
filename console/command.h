#ifndef COMMAND_H
#define COMMAND_H

#include <WString.h>

class ICommand
{
public:
    virtual ~ICommand(){}
    virtual bool IsCommand(const String& command) = 0;
    virtual void doCommand(const String& params) = 0;
};

template <typename Func>
class Command : public ICommand
{
public:
    Command(const String& command, Func func)
        : m_command(command), m_func(func){}
    virtual ~Command(){}
    
    bool IsCommand(const String& command) override
    {
        return m_command == command;
    }
    
    void doCommand(const String& params) override
    {
        m_func(params);
    }
    
private:
    String m_command;
    Func m_func;
};

template<typename Func>
Command<Func>* CreateCommand(const String& command, Func func) {
    return new Command<Func>(command, func);
}

#endif/*COMMAND_H*/