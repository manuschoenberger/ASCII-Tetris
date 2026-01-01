#include "../include/platform.hpp"
#include <windows.h>
#include <conio.h>

static DWORD originalMode = 0;
static UINT originalCPOut = 0;
static UINT originalCPIn = 0;

namespace platform {

    void init() {
        // store and set UTF-8
        originalCPOut = GetConsoleOutputCP();
        originalCPIn = GetConsoleCP();
        SetConsoleOutputCP(CP_UTF8);
        SetConsoleCP(CP_UTF8);

        // enable ANSI escape processing
        HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
        DWORD dwMode = 0;

        if (GetConsoleMode(hOut, &dwMode)) {
            originalMode = dwMode;
            dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
            SetConsoleMode(hOut, dwMode);
        }
    }

    void restore() {
        // restore code pages
        SetConsoleOutputCP(originalCPOut);
        SetConsoleCP(originalCPIn);

        // restore console mode
        HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
        SetConsoleMode(hOut, originalMode);
    }

    bool kbhit() {
        return _kbhit() != 0;
    }

    int getch() {
        return _getch();
    }
}

