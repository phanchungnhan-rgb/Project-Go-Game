#pragma once

#include <vector>
#include <iostream>
#include <string>

#ifdef __linux__

#elif _WIN32
    #define _AMD64_ 1
    #include <windows.h>
    #include <fileapi.h>
    #include <tchar.h>

    bool listFilesOnly(char *dir, std::vector<std::string> &namelist);
#elif __APPLE__

#elif __FreeBSD__

#elif __OpenBSD__

#elif __NetBSD__

#else

#endif