#ifndef _LOGGER_H_
#define _LOGGER_H_

#include "Common/cpplang.hpp"

#define LOGI(...) Logger::log(LOG_INFO, __FILE__, __LINE__, __VA_ARGS__)
#define LOGD(...) Logger::log(LOG_DEBUG, __FILE__, __LINE__, __VA_ARGS__)
#define LOGW(...) Logger::log(LOG_WARNING, __FILE__, __LINE__, __VA_ARGS__)
#define LOGE(...) Logger::log(LOG_ERROR, __FILE__, __LINE__, __VA_ARGS__)

static constexpr int MAX_LOG_LENGTH = 1024;

typedef void (*LogFunc)(void *context, int level, const char *msg);

enum LogLevel
{
    LOG_INFO,
    LOG_DEBUG,
    LOG_WARNING,
    LOG_ERROR,
};

class Logger
{
public:

    static void setLogFunc(void *ctx, LogFunc func)
    {
        s_logContext = ctx;
        s_logFunc = func;
    }

    static void setLogLevel(LogLevel level)
    {
        s_minLevel = level;
    }

    static void log(LogLevel level, const char * file, int line, const char * message, ...)
    {
        std::lock_guard<std::mutex> lock_guard(s_mutex);
        if (level < s_minLevel)
            return;

        va_list argPtr;
        va_start(argPtr, message);
        vsnprintf(s_buf, MAX_LOG_LENGTH - 1, message, argPtr);
        va_end(argPtr);
        s_buf[MAX_LOG_LENGTH - 1] = '\0';

        if (s_logFunc != nullptr )
        {
            s_logFunc(s_logContext, level, s_buf);
            return;
        }

        switch (level)
        {
#ifdef LOG_SOURCE_LINE
        case LOG_INFO:
            fprintf(stdout, "[INFO] %s:%d %s\n", file, line, s_buf);
            break;
        case LOG_DEBUG:
            fprintf(stdout, "[DEBUG] %s:%d %s\n", file, line, s_buf);
            break;
        case LOG_WARNING:
            fprintf(stdout, "[WARNING] %s:%d %s\n", file, line, s_buf);
            break;
        case LOG_ERROR:
            fprintf(stderr, "[ERROR] %s:%d %s\n", file, line, s_buf);
            break;
#else
        case LOG_INFO:
            fprintf(stdout, "[INFO] %s\n", s_buf);
            break;
        case LOG_DEBUG:
            fprintf(stdout, "[DEBUG] %s\n", s_buf);
            break;
        case LOG_WARNING:
            fprintf(stdout, "[WARNING] %s\n", s_buf);
            break;
        case LOG_ERROR:
            fprintf(stderr, "[ERROR] %s\n", s_buf);
            break;
#endif
        }
        fflush(stdout);
        fflush(stderr);
    }

private:
    static void *s_logContext;
    static LogFunc s_logFunc;
    static LogLevel s_minLevel;

    static char s_buf[MAX_LOG_LENGTH];
    static std::mutex s_mutex;
};

void *Logger::s_logContext = nullptr;
LogFunc Logger::s_logFunc = nullptr;
LogLevel Logger::s_minLevel = LOG_INFO;
char Logger::s_buf[MAX_LOG_LENGTH] = {};
std::mutex Logger::s_mutex;

#endif // _LOGGER_H_