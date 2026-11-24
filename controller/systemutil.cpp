#include "systemutil.hpp"

#ifdef __linux__

#elif _WIN32
    LPTSTR currentPath() {
        TCHAR pathbufsz[MAX_PATH];
        LPTSTR NPath = pathbufsz;
        DWORD a = GetCurrentDirectory(MAX_PATH, NPath);

        return NPath;
    }

    bool listFilesOnly(TCHAR *dir, std::vector<std::string> &namelist) {
        LPTSTR curpath = currentPath();
        TCHAR bufdir[MAX_PATH];
        _tcscpy_s(bufdir, dir);
        // _tcscat_s(bufdir, TEXT("\\"));
        // if (dir[0] == '.' && dir[1] == '\\') _tcscat_s(bufdir, dir + 2); else _tcscat_s(bufdir, dir);
        if (dir[_tcslen(dir) - 1] == '\\') _tcscat_s(bufdir, TEXT("*")); else _tcscat_s(bufdir, TEXT("\\*"));
        
        WIN32_FIND_DATA ffd; std::cout << bufdir << std::endl;
        HANDLE hFind = FindFirstFile( bufdir , &ffd);
        if (INVALID_HANDLE_VALUE == hFind) 
        {
            std::cerr << GetLastError() << std::endl;
            return false;
        } 
        do
        {
            if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            {
            }
            else
            {
                char filename[367];
                // wcstombs_s(nullptr, filename, 367, ffd.cFileName, _TRUNCATE);
                namelist.push_back(ffd.cFileName);
                std::cout << ffd.cFileName << std::endl;
            }
        }
        while (FindNextFile(hFind, &ffd) != 0);
        return true;
    }
#elif __APPLE__

#elif __FreeBSD__

#elif __OpenBSD__

#elif __NetBSD__

#else

#endif