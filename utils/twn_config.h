// #ifndef CONFIG_H
// #define CONFIG_H

#include "WString.h"
#include "vector.h"
#include "ArduinoJson.h"

class Config
{
public:
    Config();
    ~Config();
    
    void Read();
    void Write();
    
    void AddKey(const String& key);
    twnstd::vector<String> GetKeys();
    bool setValue(const String& key, const String& value);
    String getValue(const String& key);
private:
    JsonObject* m_root;
    StaticJsonBuffer<500> jsonBuffer;
    twnstd::vector<String> keys;
};

// #endif/*CONFIG_H*/
