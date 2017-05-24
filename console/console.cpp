#include <Arduino.h>
#include "console.h"

Console::Console()
{
    Serial.print("#");
}

Console::~Console()
{
}

bool Console::Write(const char* log)
{
    ClearLine();
    Serial.println(log);
    Serial.print("#");
    Serial.print(m_command);
    return true;
}

bool Console::Read(char* buf, int bufLen)
{
    int ch;
    do {
        ch = Serial.read();
        if(ch == '\n')
        {
            strcpy(buf, m_command.c_str());
            m_command = "";
            Serial.println();
            Serial.print("#");
            return true;
        } else if(ch == 27){                        // escape
            ClearLine();
            Serial.print("#");
            m_command = "";
        } else if(ch == '\t'){
        } else if(ch == 127 && m_command.length()){ // '\b'
            m_command.remove(m_command.length() - 1);
            Serial.print("\b \b");
        } else if(ch > 31 && ch < 127){
            Serial.print((char)ch);
            m_command.concat((char)ch);
        }
    } while(ch != -1);
    return false;
}

void Console::ClearLine()
{
    char simb[] = {'\b', ' ', '\b'};
    for(int count = 0; count < sizeof(simb); count++)
    {
        for(int i= 0; i < m_command.length() + 1; i++)
        {
            Serial.print(simb[count]);
        }
    }
}