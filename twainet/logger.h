#ifndef LOGGER_H
#define LOGGER_H

#define LOG_WARNING(Format, ...) Log(LOG_WARNING, Format, ## __VA_ARGS__)
#define LOG_INFO(Format, ...) Log(LOG_INFO, Format, ## __VA_ARGS__)
#define LOG_ERROR(Format, ...) Log(LOG_ERROR, Format, ## __VA_ARGS__)

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus    
    
typedef enum
{
    LOG_WARNING,
    LOG_INFO,
    LOG_ERROR
} TypeLog;

void Log(TypeLog type, const char* prototype, ...);

#ifdef __cplusplus    
}
#endif //__cplusplus    

#endif/*LOGGER_H*/
