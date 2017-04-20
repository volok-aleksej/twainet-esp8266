#ifndef CONSOLE_H
#define CONSOLE_H

#include "logger.h"
#include <WString.h>

class Console : public IConsole
{
public:
    Console();
    virtual bool Write(const char* log);
    virtual bool Read(char* buf, int bufLen);
protected:
    void ClearLine();
private:
    String m_command;
};

#endif/*CONSOLE_H*/