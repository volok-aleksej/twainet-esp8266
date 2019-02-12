// #ifndef CONFIG_H
// #define CONFIG_H

#include "WString.h"
#include "ArduinoJson.h"

class Config
{
public:
    Config();
    ~Config();
    
    void Read();
    void Write();
    
    bool setValue(const String& key, const String& value);
    bool removeValue(const String& key);
    String getValue(const String& key);
private:
    JsonObject* m_root;
    StaticJsonBuffer<500> jsonBuffer;
};

// #endif/*CONFIG_H*/
