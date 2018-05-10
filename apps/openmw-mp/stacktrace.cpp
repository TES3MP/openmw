//
// Created by koncord on 25.10.17.
//

#include "stacktrace.hpp"
#include <components/openmw-mp/Log.hpp>

#ifndef _WIN32

#include <cxxabi.h>
#include <execinfo.h>

void stacktrace()
{
    LOG_MESSAGE_SIMPLE(Log::LOG_FATAL, "Stacktrace:");

    void *array[50];
    int size = backtrace(array, 50);

    auto messages = backtrace_symbols(array, size);

    size_t funcnamesize = 256;
    auto funcname = (char *) malloc(funcnamesize);
    //skip first stack frame (points here)
    for (int i = 1; i < size && messages != nullptr; ++i)
    {
        char *beginName = nullptr, *beginOffset = nullptr, *endOffset = nullptr;
        for (char *p = messages[i]; *p; ++p)
        {
            if (*p == '(')
                beginName = p;
            else if (*p == '+')
                beginOffset = p;
            else if (*p == ')' && beginOffset)
            {
                endOffset = p;
                break;
            }
        }

        if (beginName && beginOffset && endOffset && beginName < beginOffset)
        {
            *beginName++ = '\0';
            *beginOffset++ = '\0';
            *endOffset = '\0';

            // mangled name is now in [beginName, beginOffset) and caller offset in [beginOffset, endOffset).

            int status;
            char *ret = abi::__cxa_demangle(beginName, funcname, &funcnamesize, &status);
            if (status == 0)
            {
                funcname = ret; // use possibly realloc()-ed string
                LOG_APPEND(Log::LOG_FATAL, "\t%s : %s+%s", messages[i], funcname, beginOffset);
            }
            else // demangling failed.
                LOG_APPEND(Log::LOG_FATAL, "\t%s : %s()+%s", messages[i], beginName, beginOffset);
        }
        else
            LOG_APPEND(Log::LOG_FATAL, "\t%s", messages[i]);
    }

    free(messages);
    free(funcname);
}

#else

#include "stackwalker/StackWalker.h"

class StackWalkerClr: public StackWalker
{
    std::string out;
    virtual void OnOutput(LPCSTR szText)
    {
        out.append(szText);
    }

public:
    std::string &getData()
    {
        ShowCallstack();
        return out;
    }
};


void stacktrace()
{
    LOG_MESSAGE_SIMPLE(Log::LOG_FATAL,  "Stacktrace:");
    StackWalkerClr swc;
    LOG_APPEND(Log::LOG_FATAL, swc.getData().c_str());
}

#endif
