#ifndef TEST_MAIN_H
#define TEST_MAIN_H

#include <cstdio> // snprintf(x)

#if defined _windows
	#include <windows.h>
#endif

#include <libsdl2gui.h>

template<typename... Args>
static std::string TextFormat(const char* formatString, const Args&... args)
{
    if (!formatString)
        return "";

    char buffer[1024] = {};
    std::snprintf(buffer, 1024, formatString, args...);

    return std::string(buffer);
}

#endif
