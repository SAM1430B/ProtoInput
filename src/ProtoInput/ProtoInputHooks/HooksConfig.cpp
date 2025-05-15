#include "HooksConfig.h"
#include <unordered_map>
#include <sstream>

namespace HooksConfig
{
    Config gConfig;

    // Define a map of style flag names to their values
    static const std::unordered_map<std::string, DWORD> styleFlags = {
        {"WS_BORDER", WS_BORDER},
        {"WS_SYSMENU", WS_SYSMENU},
        {"WS_DLGFRAME", WS_DLGFRAME},
        {"WS_CAPTION", WS_CAPTION},
        {"WS_THICKFRAME", WS_THICKFRAME},
        {"WS_MINIMIZEBOX", WS_MINIMIZEBOX},
        {"WS_MAXIMIZEBOX", WS_MAXIMIZEBOX},
        {"WS_VISIBLE", WS_VISIBLE},
        {"WS_POPUP", WS_POPUP},
        {"WS_CLIPCHILDREN", WS_CLIPCHILDREN},
        {"WS_CLIPSIBLINGS", WS_CLIPSIBLINGS}
    };

    DWORD ParseStyleFlags(const char* styleString)
    {
        if (!styleString || !*styleString)
            return 0;

        DWORD result = 0;
        std::string input(styleString);
        std::istringstream stream(input);
        std::string token;

        while (std::getline(stream, token, '|')) {
            // Trim whitespace
            token.erase(0, token.find_first_not_of(" \t"));
            token.erase(token.find_last_not_of(" \t") + 1);

            auto it = styleFlags.find(token);
            if (it != styleFlags.end()) {
                result |= it->second;
            }
            // Could add else with error logging if needed
        }

        return result;
    }

    Config ReadConfigFromIni()
    {
        Config config;
        // Initialize the config with default values
        memset(config.windowTitle, 0, sizeof(config.windowTitle));
        config.sendMouseDblClkMessages = true;
        config.windowBoundsFix = true;
        config.noResize = false;
        config.noReposition = false;
        config.moveWindow = false;
        config.adjustWindowRect = false;
        config.adjustWindowRectEx = false;
        config.mouseBoundsWidth = 100;
        config.mouseBoundsHeight = 100;
        config.cursorSize = 40;
        config.drawCursorFix = true;

        // Get paths
        char exePath[MAX_PATH];
        GetModuleFileNameA(NULL, exePath, MAX_PATH);

        // Construct paths
        std::string exePathStr(exePath);
        std::string exeDir = exePathStr.substr(0, exePathStr.find_last_of('\\') + 1);

        // Create both possible INI paths
        std::string exeNameIni = exePathStr;
        size_t lastDot = exeNameIni.find_last_of('.');
        if (lastDot != std::string::npos) {
            exeNameIni.replace(lastDot, exeNameIni.length() - lastDot, ".ini");
        }
        else {
            exeNameIni += ".ini";
        }

        std::string specificIni = exeDir + "Proto.Config.ini";

        // Check if files exist directly
        bool specificIniExists = (GetFileAttributesA(specificIni.c_str()) != INVALID_FILE_ATTRIBUTES);
        bool exeIniExists = (GetFileAttributesA(exeNameIni.c_str()) != INVALID_FILE_ATTRIBUTES);

        // Decide which INI to use
        std::string iniToUse;
        if (specificIniExists) {
            iniToUse = specificIni;
            // OutputDebugStringA("Using WinStyle.ini");
        }
        else if (exeIniExists) {
            iniToUse = exeNameIni;
            // OutputDebugStringA("Using executable-named INI");
        }
        else {
            // No existing INI found, create a default one
            iniToUse = specificIni;
            // OutputDebugStringA("Creating new WinStyle.ini");

            // Create with default values
            WritePrivateProfileStringA("WindowStyleHook", "WindowTitle", "", specificIni.c_str());
            WritePrivateProfileStringA("WindowStyleHook", "RemoveStyle", "WS_BORDER | WS_SYSMENU | WS_DLGFRAME | WS_CAPTION | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX", specificIni.c_str());
            WritePrivateProfileStringA("WindowStyleHook", "AddStyle", "WS_VISIBLE | WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS", specificIni.c_str());
            WritePrivateProfileStringA("RawInput", "SendMouseDblClkMessages", "1", specificIni.c_str());
            WritePrivateProfileStringA("GetCursorPosHook", "WindowBoundsFix", "1", specificIni.c_str());
            WritePrivateProfileStringA("SetWindowPosHook", "NoResize", "0", specificIni.c_str());
            WritePrivateProfileStringA("SetWindowPosHook", "NoRepostion", "0", specificIni.c_str());
            WritePrivateProfileStringA("SetWindowPosHook", "MoveWindow", "0", specificIni.c_str());
            WritePrivateProfileStringA("SetWindowPosHook", "AdjustWindowRect", "0", specificIni.c_str());
            WritePrivateProfileStringA("SetWindowPosHook", "AdjustWindowRectEx", "0", specificIni.c_str());
            WritePrivateProfileStringA("SetWindowPosHook", "AdjustWindowRectEx", "0", specificIni.c_str());
            WritePrivateProfileStringA("ExtendFakeCursorBounds", "MouseBoundsWidth", "100", specificIni.c_str());
            WritePrivateProfileStringA("ExtendFakeCursorBounds", "MouseBoundsHeight", "100", specificIni.c_str());
            WritePrivateProfileStringA("FakeCursor", "CursorSize", "40", specificIni.c_str());
            WritePrivateProfileStringA("FakeCursor", "DrawCursorFix", "1", specificIni.c_str());
        }



        // Read settings
        char buffer[512];
        if (GetPrivateProfileStringA("WindowStyleHook", "WindowTitle", "", buffer, sizeof(buffer), iniToUse.c_str()) > 0) {
            // Safely copy the string to prevent buffer overflows
            strncpy(config.windowTitle, buffer, sizeof(config.windowTitle) - 1);
            config.windowTitle[sizeof(config.windowTitle) - 1] = '\0'; // Ensure null termination
        }

        if (GetPrivateProfileStringA("WindowStyleHook", "RemoveStyle", "", buffer, sizeof(buffer), iniToUse.c_str()) > 0) {
            config.removeStyle = ParseStyleFlags(buffer);
        }

        if (GetPrivateProfileStringA("WindowStyleHook", "AddStyle", "", buffer, sizeof(buffer), iniToUse.c_str()) > 0) {
            config.addStyle = ParseStyleFlags(buffer);
        }

        // Read boolean values
        config.sendMouseDblClkMessages = GetPrivateProfileIntA("RawInput", "SendMouseDblClkMessages", 1, iniToUse.c_str()) != 0;
        config.windowBoundsFix = GetPrivateProfileIntA("GetCursorPosHook", "WindowBoundsFix", 1, iniToUse.c_str()) != 0;
        config.noResize = GetPrivateProfileIntA("SetWindowPosHook", "NoResize", 0, iniToUse.c_str()) != 0;
        config.noReposition = GetPrivateProfileIntA("SetWindowPosHook", "NoReposition", 0, iniToUse.c_str()) != 0;
        config.moveWindow = GetPrivateProfileIntA("SetWindowPosHook", "MoveWindow", 0, iniToUse.c_str()) != 0;
        config.adjustWindowRect = GetPrivateProfileIntA("SetWindowPosHook", "AdjustWindowRect", 0, iniToUse.c_str()) != 0;
        config.adjustWindowRectEx = GetPrivateProfileIntA("SetWindowPosHook", "AdjustWindowRectEx", 0, iniToUse.c_str()) != 0;
        config.mouseBoundsWidth = GetPrivateProfileIntA("ExtendFakeCursorBounds", "MouseBoundsWidth", 100, iniToUse.c_str());
        config.mouseBoundsHeight = GetPrivateProfileIntA("ExtendFakeCursorBounds", "MouseBoundsHeight", 100, iniToUse.c_str());
        config.cursorSize = GetPrivateProfileIntA("FakeCursor", "CursorSize", 40, iniToUse.c_str());
        config.drawCursorFix = GetPrivateProfileIntA("FakeCursor", "DrawCursorFix", 1, iniToUse.c_str()) != 0;

        return config;
    }

    /*void HooksConfig::InstallImpl()
    {
        //HooksConfig::gConfig = HooksConfig::ReadConfigFromIni();
    }*/
}