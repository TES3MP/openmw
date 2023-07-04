#ifndef CRASHCATCHER_H
#define CRASHCATCHER_H

#include <filesystem>
#include <string>

<<<<<<< HEAD
#if (defined(__APPLE__) || (defined(__linux)  &&  !defined(ANDROID)) || (defined(__unix) &&  !defined(ANDROID)) || defined(__posix))
    #define USE_CRASH_CATCHER 0
=======
#if (defined(__APPLE__) || (defined(__linux) && !defined(ANDROID)) || (defined(__unix) && !defined(ANDROID))           \
    || defined(__posix))
#define USE_CRASH_CATCHER 1
>>>>>>> 8a33edd64a6f0e9fe3962c88618e8b27aad1b7a7
#else
#define USE_CRASH_CATCHER 0
#endif

constexpr char crash_switch[] = "--cc-handle-crash";

#if USE_CRASH_CATCHER
extern void crashCatcherInstall(int argc, char** argv, const std::filesystem::path& crashLogPath);
#else
inline void crashCatcherInstall(int, char**, const std::string& crashLogPath) {}
#endif

#endif
