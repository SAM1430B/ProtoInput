#include "GetCursorPosHook.h"
#include "FakeMouseKeyboard.h"
#include "HwndSelector.h"
#include "HooksConfig.h" // temp

namespace Proto
{

    BOOL WINAPI Hook_GetCursorPos(LPPOINT lpPoint)
    {
        if (lpPoint)
        {
            const auto& state = FakeMouseKeyboard::GetMouseState();
            lpPoint->x = state.x;
            lpPoint->y = state.y;

            if (HooksConfig::gConfig.windowBoundsFix) {

                // Get the stored window bounds using the static class members
                int clientWidth = HwndSelector::windowWidth;
                int clientHeight = HwndSelector::windowHeight;

                /*  // Handle DPI scaling
                HWND hwnd = (HWND)HwndSelector::GetSelectedHwnd();
                UINT dpi = GetDpiForWindow(hwnd);
                float dpiScale = dpi / 96.0f;

                // Define edge threshold considering DPI
                int windowEdge = static_cast<int>(dpiScale);*/

                // without dpi
                /*const int windowEdge = 1;

                // Top and left edges - these are working properly
                if (lpPoint->y < windowEdge)
                    lpPoint->y = 0;  // Top edge

                if (lpPoint->x < windowEdge)
                    lpPoint->x = 0;  // Left edge

                // Bottom and right edges - try a different approach
                if (lpPoint->y > clientHeight - windowEdge)
                    lpPoint->y = clientHeight - 1;  // Bottom edge, -1 to be just inside

                if (lpPoint->x > clientWidth - windowEdge)
                    lpPoint->x = clientWidth - 1;  // Right edge, -1 to be just inside*/
                              
                // Top and left edges - these are working properly
                if (lpPoint->y < 1)
                    lpPoint->y = 0;  // Top edge

                if (lpPoint->x < 1)
                    lpPoint->x = 0;  // Left edge

                // Bottom and right edges - try a different approach
                if (lpPoint->y > clientHeight - 1)
                    lpPoint->y = clientHeight - 1;  // Bottom edge, -1 to be just inside

                if (lpPoint->x > clientWidth - 1)
                    lpPoint->x = clientWidth - 1;  // Right edge, -1 to be just inside
            }

            // Convert to screen coordinates
            ClientToScreen((HWND)HwndSelector::GetSelectedHwnd(), lpPoint);
        }

        return true;
    }

void GetCursorPosHook::ShowGuiStatus()
{
	
}

void GetCursorPosHook::InstallImpl()
{
	hookInfo = std::get<1>(InstallNamedHook(L"user32", "GetCursorPos", Hook_GetCursorPos));
}

void GetCursorPosHook::UninstallImpl()
{
	UninstallHook(&hookInfo);
}

}
