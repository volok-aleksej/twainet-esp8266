// #ifndef CONFIG_H
// #define CONFIG_H

#include "WString.h"
#include "ArduinoJson.h"
#include "vector.h"

class Config
{
public:
    Config();
    ~Config();
    
    void Read();
    void Write();
    
    void addKey(const char* key);
    twnstd::vector<const char*> getKeys();
    
    bool setValue(const char* key, const char* value);
    bool removeValue(const char* key);
    const char* getValue(const char* key);
private:
    JsonObject* m_root;
    StaticJsonBuffer<500> jsonBuffer;
    twnstd::vector<const char*> keys;
};

// #endif/*CONFIG_H*/
