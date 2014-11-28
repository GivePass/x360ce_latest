#pragma once

// C++ headers
#include <cstdio>
#include <cstdlib>
#include <string>
#include <memory>

#include <io.h>
#include <fcntl.h> 
#include <windows.h> 

// Windows headers
#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")

#include "Utils.h"
#include "Mutex.h"
#include "NonCopyable.h"

// warning C4127: conditional expression is constant
#pragma warning(disable: 4127)

#ifndef DISABLE_LOGGER
#define INITIALIZE_LOGGER Logger* Logger::m_instance;
class Logger : NonCopyable
{
public:
    static Logger* GetInstance()
    {
        if (!m_instance) m_instance = new Logger;
        return m_instance;
    }

    virtual ~Logger()
    {
        if (m_console != nullptr)
        {
            FreeConsole();
            CloseHandle(m_console);
        }

        if (m_file != nullptr)
        {
            CloseHandle(m_file);
        }

        delete m_instance;
    }

    bool file(const char* filename)
    {
        std::string logpath;
        if (PathIsRelativeA(filename))
        {
            char tmp_logpath[MAX_PATH];
            DWORD dwLen = GetModuleFileNameA(CurrentModule(), tmp_logpath, MAX_PATH);
            if (dwLen > 0 && PathRemoveFileSpecA(tmp_logpath))
                PathAppendA(tmp_logpath, filename);
            logpath = tmp_logpath;
        }

        m_file = CreateFileA(logpath.c_str(), GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        return m_file != INVALID_HANDLE_VALUE;
    }

    bool console(const char* title = nullptr, const char* console_notice = nullptr)
    {
        if (AllocConsole())
        {
            m_console = GetStdHandle(STD_OUTPUT_HANDLE);
            if (m_console != INVALID_HANDLE_VALUE)
            {
                ShowWindow(GetConsoleWindow(), SW_MAXIMIZE);
                if (title) SetConsoleTitleA(title);
                if (console_notice)
                {
                    DWORD len = (DWORD)strlen(console_notice);
                    DWORD lenout = 0;
                    WriteConsoleA(m_console, console_notice, len, &lenout, NULL);
                }
            }
            return m_console != INVALID_HANDLE_VALUE;
        }
        return false;
    }

    void print_timestamp(bool file, bool console, const char* format, ...)
    {
        if ((file || console) && format)
        {
            char buffer[1024];

            va_list arglist;
            va_start(arglist, format);
            vsprintf_s(buffer, format, arglist);
            va_end(arglist);

            DWORD len = (DWORD)strlen(buffer);
            DWORD lenout = 0;

            if (console) WriteConsoleA(m_console, buffer, len, &lenout, NULL);
            if (file) WriteFile(m_file, buffer, len, &lenout, NULL);
        }
    }

    void print(const char* format, va_list vaargs)
    {
        bool log = m_file != INVALID_HANDLE_VALUE;
        bool con = m_console != INVALID_HANDLE_VALUE;

        if ((log || con) && format)
        {
            LockGuard lock(m_mtx);

            DWORD len = 0;
            DWORD lenout = 0;
            static char* stamp = "[TIME]\t\t[THREAD]\t[LOG]\n";
            if (stamp)
            {
                len = (DWORD)strlen(stamp);
                if (con) WriteConsoleA(m_console, stamp, len, &lenout, NULL);
                if (log) WriteFile(m_file, stamp, len, &lenout, NULL);
                stamp = nullptr;
            }

            GetLocalTime(&m_systime);
            print_timestamp(log, con, "%02u:%02u:%02u.%03u\t%08u\t", m_systime.wHour, m_systime.wMinute,
                m_systime.wSecond, m_systime.wMilliseconds, GetCurrentThreadId());

            vsnprintf_s(m_print_buffer, 1024, 1024, format, vaargs);
            strncat_s(m_print_buffer, 1024, "\r\n", _TRUNCATE);

            len = (DWORD)strlen(m_print_buffer);
            lenout = 0;

            if (con) WriteConsoleA(m_console, m_print_buffer, len, &lenout, NULL);
            if (log) WriteFile(m_file, m_print_buffer, len, &lenout, NULL);

        }
    }
private:
    static Logger* m_instance;
    char m_print_buffer[1024];

    SYSTEMTIME m_systime;
    HANDLE m_console;
    HANDLE m_file;

    Mutex m_mtx;

    Logger()
    {
        m_file = nullptr;
        m_console = nullptr;
    }
};

inline void LogFile(const char* logname)
{
    Logger::GetInstance()->file(logname);
}

inline void LogConsole(const char* title = nullptr, const char* console_notice = nullptr)
{
    Logger::GetInstance()->console(title, console_notice);
}

inline void PrintLog(const char* format, ...)
{
    va_list vaargs;
    va_start(vaargs, format);
    Logger::GetInstance()->print(format, vaargs);
    va_end(vaargs);
}

#define PrintFunc() PrintLog(__FUNCTION__)
#define PrintFuncSig() PrintLog(__FUNCSIG__)

#else
#define INITIALIZE_LOGGER
#define LogFile(logname)
#define LogConsole(title, notice)
#define PrintLog(format, ...)
#define PrintFunc()
#define PrintFuncSig()

#endif