#include <Arduino.h>
#include "console.h"
#include "command_line.h"
#include "utils.h"

Console::Console()
: m_stream(0){}

void Console::Init()
{
    m_stream = &Serial;
    m_stream->print("#");
}

Console::~Console()
{
}

bool Console::Write(const char* log)
{
    ClearLine();
    if(m_stream) {
        m_stream->println(log);
        m_stream->print("#");
        m_stream->print(m_command);
    }
    return true;
}

bool Console::Read(char* buf, int bufLen)
{
    if(!m_stream) {
        return false;
    }
        
    int ch;
    do {
        ch = m_stream->read();
        if(ch == '\n')
        {
            strcpy(buf, m_command.c_str());
            m_command = "";
            m_stream->println();
            m_stream->print("#");
            return true;
        } else if(ch == 27){                        // escape
            ClearLine();
            m_stream->print("#");
            m_command = "";
        } else if(ch == '\t'){
            bool new_word;
            twnstd::vector<String> params = getSubstrings(m_command, " ");
            twnstd::vector<String> args = CommandLine::GetInstance().GetNextCommandArgs(params, new_word);
            if(new_word) {
                params.push_back("");
            }
            args = autoCompleteHelper(params.length() ? params.back() : "", args);
            ClearLine();
            if(args.length() == 1) {
                params.pop_back();
                params.push_back(args[0]);
                m_command = "";
                for(int i = 0; i < params.length(); i++) {
                    m_command += params[i];
                    m_command += " ";
                }
            } else {
                for(int i = 0; i < args.length(); i++) {
                    m_stream->print(args[i].c_str());
                    m_stream->print(" ");
                }
                m_stream->println();
            }
            m_stream->print("#");
            m_stream->print(m_command);
        } else if(ch == '\r'){
        } else if(ch == '\b' && m_command.length()){
            m_command.remove(m_command.length() - 1);
            m_stream->print("\b \b");
        } else if(ch > 31 && ch < 127){
            m_stream->print((char)ch);
            m_command.concat((char)ch);
        }
    } while(ch != -1);
    return false;
}

void Console::ClearLine()
{
    if(!m_stream) {
        return;
    }
       
    for(int i= 0; i < m_command.length() + 1; i++)
    {
        m_stream->print("\b \b");
    }
}
