#pragma once
//#include "Hook.h"
#include <Windows.h>
//#include <string>
#include <fstream>


namespace HooksConfig
{
    struct Config {
        char windowTitle[256];
        DWORD removeStyle;
        DWORD addStyle;
        bool windowBoundsFix = true;
        bool sendMouseDblClkMessages = true;
        bool noResize = false;
        bool noReposition = false;
        bool moveWindow = false;
        bool adjustWindowRect = false;
        bool adjustWindowRectEx = false;
        int mouseBoundsWidth;
        int mouseBoundsHeight;
        int cursorSize;
        bool drawCursorFix = true;
    };

    //extern bool gConfigInitialized;

    extern Config gConfig;
    DWORD ParseStyleFlags(const char* styleString);

    Config ReadConfigFromIni();
    //void InstallImpl() override;
}