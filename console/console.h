#ifndef CONSOLE_H
#define CONSOLE_H

#include <WString.h>
#include <Stream.h>
#include "logger.h"

class Console : public IConsole
{
public:
    Console();
    void Init() override;
    bool Write(const char* log) override;
    bool Read(char* buf, int bufLen) override;
protected:
    void ClearLine();
private:
    String m_command;
    Stream* m_stream;
};

#endif/*CONSOLE_H*/