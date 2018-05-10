#include "config.h"
#include "FS.h"
#include "logger.h"
#include "vector.h"
#include "utils.h"

Config::Config() : m_root(0){}

Config::~Config(){}
    
void Config::Read()
{
    //DynamicJsonBuffer jsonBuffer;
    if(SPIFFS.exists("/config.txt")) {
        File fp = SPIFFS.open("/config.txt", "r");
        String line = fp.readStringUntil('\n');
        m_root = &jsonBuffer.parseObject( line );
        fp.close();
    } else {
        m_root = &jsonBuffer.createObject();
        LOG_WARNING("config file not exists");
    }
}

void Config::Write()
{
    if(!m_root) {
        return;
    }
    
    String data;
    m_root->printTo(data);
    LOG_INFO("%s", data.c_str());
    File fp = SPIFFS.open("/config.txt", "w");
    fp.write((uint8_t*)data.c_str(), data.length());
    fp.close();
    
}

bool Config::setValue(const String& key, const String& value)
{
    if(!m_root) {
        return false;
    }
    
    JsonObject* obj = m_root;
    twnstd::vector<String> keys = getSubstrings(key, ".");
    for(int i = 0; i < keys.length(); i++) {
        if(i + 1 < keys.length() && obj->containsKey(keys[i])) {
            JsonObject& newobj = obj->get<JsonVariant>(keys[i]);
            obj = &newobj;
        } else if(i + 1 < keys.length() && !obj->containsKey(keys[i])) {
            obj = &(obj->createNestedObject(keys[i]));
        } else {
            return obj->set(keys[i], value);
        }
    }
    
    return false;
}

String Config::getValue(const String& key)
{
    if(!m_root) {
        return "";
    }
    
    twnstd::vector<String> keys = getSubstrings(key, ".");
    JsonObject* obj = m_root;
    for(int i = 0; i < keys.length(); i++) {
        if(i + 1 < keys.length() && obj->containsKey(keys[i])) {
            JsonObject& newobj = obj->get<JsonVariant>(keys[i]);
            obj = &newobj;
        } else if(obj->containsKey(keys[i])){
            const char* data = obj->get<const char*>(keys[i]);
            return data;
        } else {
            LOG_INFO("absent key");
            break;
        }
    }
    return "";
}
