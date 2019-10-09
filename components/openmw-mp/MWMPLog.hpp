//
// Created by koncord on 15.08.16.
//

#ifndef OPENMW_LOG_HPP
#define OPENMW_LOG_HPP

#include <boost/filesystem.hpp>

#ifdef __GNUC__
#pragma GCC system_header
#endif

#if defined(NOLOGS)
#define LOG_INIT(logLevel)
#define LOG_QUIT()
#define LOG_MESSAGE(level, msg, ...)
#define LOG_MESSAGE_SIMPLE(level, msg, ...)
#else
#define LOG_INIT(logLevel) MWMPLog::Create(logLevel)
#define LOG_QUIT() MWMPLog::Delete()
#if defined(_MSC_VER)
#define LOG_MESSAGE(level, msg, ...) MWMPLog::Get().print((level), (1), (__FILE__), (__LINE__), (msg), __VA_ARGS__)
#define LOG_MESSAGE_SIMPLE(level, msg, ...) MWMPLog::Get().print((level), (1), (0), (0), (msg), __VA_ARGS__)
#define LOG_APPEND(level, msg, ...) MWMPLog::Get().print((level), (0), (0), (0), (msg), __VA_ARGS__)
#else
#define LOG_MESSAGE(level, msg, args...) MWMPLog::Get().print((level), (1), (__FILE__), (__LINE__), (msg), ##args)
#define LOG_MESSAGE_SIMPLE(level, msg, args...) MWMPLog::Get().print((level), (1), (0), (0), (msg), ##args)
#define LOG_APPEND(level, msg, args...) MWMPLog::Get().print((level), (0), (0), (0), (msg), ##args)
#endif
#endif

class MWMPLog
{
public:
    enum
    {
        LOG_VERBOSE = 0,
        LOG_INFO,
        LOG_WARN,
        LOG_ERROR,
        LOG_FATAL
    };
    static void Create(int logLevel);
    static void Delete();
    static const MWMPLog &Get();
    static int GetLevel();
    static void SetLevel(int level);
    void print(int level, bool hasPrefix, const char *file, int line, const char *message, ...) const;

    static std::string getFilenameTimestamp();
private:
    MWMPLog(int logLevel);
    /// Not implemented
    MWMPLog(const MWMPLog &) = delete;
    /// Not implemented
    MWMPLog &operator=(MWMPLog &) = delete;
    static MWMPLog *sLog;
    int logLevel;
};


#endif //OPENMW_LOG_HPP
