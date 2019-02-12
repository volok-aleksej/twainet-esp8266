#include "twn_config.h"
#include "FS.h"
#include "logger.h"
#include "vector.h"
#include "utils.h"

char config_file[] = "/config.txt";

Config::Config() : m_root(0){}

Config::~Config(){}

void Config::addKey(const char* key)
{
    keys.push_back(key);
}

twnstd::vector<const char*> Config::getKeys()
{
    return keys;
}

void Config::Read()
{
    //DynamicJsonBuffer jsonBuffer;
    if(SPIFFS.exists(config_file)) {
        File fp = SPIFFS.open(config_file, "r");
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
    File fp = SPIFFS.open(config_file, "w");
    fp.write((uint8_t*)data.c_str(), data.length());
    fp.close();
    
}

bool Config::setValue(const char* key, const char* value)
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
            return obj->set(keys[i], String(value));
        }
    }
    
    return false;
}

const char* Config::getValue(const char* key)
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

bool Config::removeValue(const char* key)
{
    if(!m_root) {
        return false;
    }
    
    twnstd::vector<String> keys = getSubstrings(key, ".");
    JsonObject* obj = m_root;
    for(int i = 0; i < keys.length(); i++) {
        if(i + 1 < keys.length() && obj->containsKey(keys[i])) {
            JsonObject& newobj = obj->get<JsonVariant>(keys[i]);
            obj = &newobj;
        } else if(obj->containsKey(keys[i])){
            obj->remove(keys[i]);
            return true;
        } else {
            LOG_INFO("absent key");
            break;
        }
    }
    return false;
}
