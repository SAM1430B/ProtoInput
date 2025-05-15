#include "WindowStyleHook.h"



namespace Proto
{
    inline void FilterStyle(LONG& style)
    {
        style &= ~(HooksConfig::gConfig.removeStyle);

        style |= HooksConfig::gConfig.addStyle;
        /*style &= ~(WS_BORDER | WS_SYSMENU | WS_DLGFRAME | WS_CAPTION |
            WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX);

        style |= WS_VISIBLE | WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;*/
    }

    inline void FilterStylePtr(LONG_PTR& style)
    {
        style &= ~(HooksConfig::gConfig.removeStyle);

        style |= HooksConfig::gConfig.addStyle;
        /*style &= ~(WS_BORDER | WS_SYSMENU | WS_DLGFRAME | WS_CAPTION |
            WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX);

        style |= WS_VISIBLE | WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;*/

    }

    HWND GetGameWindow()
    {
        DWORD processId = GetCurrentProcessId();
        HWND gameWindow = NULL;

        if (HooksConfig::gConfig.windowTitle[0] != '\0') {
            //FindWindowA(lpClassName, lpWindowName); 'lpWindowName' should be the window title name.
            gameWindow = FindWindowA(NULL, HooksConfig::gConfig.windowTitle);
        }
        else
        {
            gameWindow = FindWindowA(NULL, NULL);
        }
        while (gameWindow)
        {
            DWORD windowProcessId;
            GetWindowThreadProcessId(gameWindow, &windowProcessId);

            if (windowProcessId == processId)
            {
                if (GetWindow(gameWindow, GW_OWNER) == NULL && IsWindowVisible(gameWindow))
                {
                    return gameWindow;
                }
            }
            gameWindow = GetWindow(gameWindow, GW_HWNDNEXT);
        }
        return NULL;
    }

    void RemoveWindowStyle(HWND hWnd)
    {
        if (!hWnd) return;

        // Get the current window rect and client rect
        RECT windowRect, clientRect;
        GetWindowRect(hWnd, &windowRect);
        GetClientRect(hWnd, &clientRect);

        // Convert client rect to screen coordinates to get the actual game content position
        POINT clientTopLeft = { clientRect.left, clientRect.top };
        ClientToScreen(hWnd, &clientTopLeft);

        // Store client area dimensions
        int clientWidth = clientRect.right - clientRect.left;
        int clientHeight = clientRect.bottom - clientRect.top;

        // Get and modify current style
        LONG currentStyle = GetWindowLong(hWnd, GWL_STYLE);
        LONG newStyle = currentStyle;
        FilterStyle(newStyle);

        // Calculate new window rectangle to maintain the same client area
        RECT newWindowRect = { 0, 0, clientWidth, clientHeight };
        AdjustWindowRectEx(&newWindowRect, newStyle, FALSE, GetWindowLong(hWnd, GWL_EXSTYLE));

        // Set the new style
        SetWindowLong(hWnd, GWL_STYLE, newStyle);

        // Update the window size and position
        // Use clientTopLeft coordinates to maintain the game content position
        SetWindowPos(hWnd,
            NULL,
            clientTopLeft.x, // Position where the client area was
            clientTopLeft.y,
            newWindowRect.right - newWindowRect.left,
            newWindowRect.bottom - newWindowRect.top,
            SWP_NOZORDER | SWP_FRAMECHANGED);
    }

    LONG WINAPI Hook_SetWindowLongA(
        HWND hWnd,
        int  nIndex,
        LONG dwNewLong
    )
    {
        if (nIndex == GWL_STYLE)
        {
            FilterStyle(dwNewLong);
        }
        return SetWindowLongA(hWnd, nIndex, dwNewLong);
    }

    LONG WINAPI Hook_SetWindowLongW(
        HWND hWnd,
        int  nIndex,
        LONG dwNewLong
    )
    {
        if (nIndex == GWL_STYLE)
        {
            FilterStyle(dwNewLong);
        }
        return SetWindowLongW(hWnd, nIndex, dwNewLong);
    }

    LONG_PTR WINAPI Hook_SetWindowLongPtrA(
        HWND hWnd,
        int  nIndex,
        LONG_PTR dwNewLong
    )
    {
        if (nIndex == GWL_STYLE)
        {
            FilterStylePtr(dwNewLong);
        }
        return SetWindowLongPtrA(hWnd, nIndex, dwNewLong);
    }

    LONG_PTR WINAPI Hook_SetWindowLongPtrW(
        HWND hWnd,
        int  nIndex,
        LONG_PTR dwNewLong
    )
    {
        if (nIndex == GWL_STYLE)
        {
            FilterStylePtr(dwNewLong);
        }
        return SetWindowLongPtrW(hWnd, nIndex, dwNewLong);
    }

    void WindowStyleHook::InstallImpl()
    {
        /*if (!HooksConfig::gConfigInitialized) {
            HooksConfig::gConfig = HooksConfig::ReadConfigFromIni();
            HooksConfig::gConfigInitialized = true;
        }*/

        HooksConfig::gConfig = HooksConfig::ReadConfigFromIni();


        hookInfoA = std::get<1>(InstallNamedHook(L"user32", "SetWindowLongA", Hook_SetWindowLongA));
        hookInfoW = std::get<1>(InstallNamedHook(L"user32", "SetWindowLongW", Hook_SetWindowLongW));
        hookInfoPtrA = std::get<1>(InstallNamedHook(L"user32", "SetWindowLongPtrA", Hook_SetWindowLongPtrA));
        hookInfoPtrW = std::get<1>(InstallNamedHook(L"user32", "SetWindowLongPtrW", Hook_SetWindowLongPtrW));
        //Sleep(50);

        // Find and modify the game window
        if (HWND gameWindow = GetGameWindow())
        {
            RemoveWindowStyle(gameWindow);
        }
    }

    void WindowStyleHook::UninstallImpl()
    {
        UninstallHook(&hookInfoA);
        UninstallHook(&hookInfoW);
        UninstallHook(&hookInfoPtrA);
        UninstallHook(&hookInfoPtrW);
    }

}
