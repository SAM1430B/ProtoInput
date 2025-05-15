#include "SetWindowPosHook.h"
#include <imgui.h>

namespace Proto
{

int SetWindowPosHook::width = 0;
int SetWindowPosHook::height = 0;
int SetWindowPosHook::posx = 0;
int SetWindowPosHook::posy = 0;
//int setupapid;

RECT SetWindowPosHook::rmonitor;
RECT SetWindowPosHook::rwork;

BOOL WINAPI Hook_SetWindowPos(HWND hWnd, HWND hWndInsertAfter, int X, int Y, int cx, int cy, UINT uFlags)
{
	int width = HooksConfig::gConfig.noResize ? cx : SetWindowPosHook::width;
	int height = HooksConfig::gConfig.noResize ? cy : SetWindowPosHook::height;
	int posx = HooksConfig::gConfig.noReposition ? X : SetWindowPosHook::posx;
	int posy = HooksConfig::gConfig.noReposition ? Y : SetWindowPosHook::posy;

	return SetWindowPos(hWnd, hWndInsertAfter, posx, posy, width, height, uFlags);
}

BOOL WINAPI MoveWindow_Hook(HWND hWnd, int X, int Y, int cx, int cy, bool bRepaint)
{
	bool result = MoveWindow(hWnd, SetWindowPosHook::posx, SetWindowPosHook::posy, SetWindowPosHook::width, SetWindowPosHook::height, bRepaint);

	return result;
}

BOOL WINAPI GetMonitorInfo_hook(HMONITOR hmonitor, LPMONITORINFO lpmi) {

	//direct change
	lpmi->rcMonitor.top = SetWindowPosHook::posy;
	lpmi->rcMonitor.left = SetWindowPosHook::posx;
	lpmi->rcMonitor.bottom = SetWindowPosHook::height + SetWindowPosHook::posy;
	lpmi->rcMonitor.right = SetWindowPosHook::width + SetWindowPosHook::posx;

	lpmi->rcWork.top = SetWindowPosHook::posy;
	lpmi->rcWork.left = SetWindowPosHook::posx;
	lpmi->rcWork.bottom = SetWindowPosHook::height + SetWindowPosHook::posy;
	lpmi->rcWork.right = SetWindowPosHook::width + SetWindowPosHook::posx;

	// or indirect change
	SetWindowPosHook::rmonitor = lpmi->rcMonitor;
	SetWindowPosHook::rwork = lpmi->rcWork;

	SetWindowPosHook::rmonitor.top = SetWindowPosHook::posy;
	SetWindowPosHook::rmonitor.left = SetWindowPosHook::posx;
	SetWindowPosHook::rmonitor.bottom = SetWindowPosHook::height + SetWindowPosHook::posy;
	SetWindowPosHook::rmonitor.right = SetWindowPosHook::width + SetWindowPosHook::posx;

	SetWindowPosHook::rwork.top = SetWindowPosHook::posy;
	SetWindowPosHook::rwork.left = SetWindowPosHook::posx;
	SetWindowPosHook::rwork.bottom = SetWindowPosHook::height + SetWindowPosHook::posy;
	SetWindowPosHook::rwork.right = SetWindowPosHook::width + SetWindowPosHook::posx;

	lpmi->rcMonitor = SetWindowPosHook::rmonitor;
	lpmi->rcWork = SetWindowPosHook::rwork;


	bool result = GetMonitorInfo_hook(hmonitor, lpmi);
	return result;
}

BOOL WINAPI AdjustWindowRect_Hook(LPRECT lpRect, DWORD dwStyle, bool bMenu)
{
	lpRect->top = SetWindowPosHook::posy;
	lpRect->left = SetWindowPosHook::posx;
	lpRect->bottom = SetWindowPosHook::height + SetWindowPosHook::posy;
	lpRect->right = SetWindowPosHook::width + SetWindowPosHook::posx;
	bool result = AdjustWindowRect(lpRect, dwStyle, bMenu);

	return result;
}
BOOL WINAPI AdjustWindowRectEx_Hook(LPRECT lpRect, DWORD dwStyle, bool bMenu, DWORD dwExStyle)
{
	lpRect->top = SetWindowPosHook::posy;
	lpRect->left = SetWindowPosHook::posx;
	lpRect->bottom = SetWindowPosHook::height + SetWindowPosHook::posy;
	lpRect->right = SetWindowPosHook::width + SetWindowPosHook::posx;

	bool result = AdjustWindowRectEx(lpRect, dwStyle, bMenu, dwExStyle);
	return result;
}


//BOOL WINSETUPAPI SetupDiEnumDeviceInterfaces_Hook( HDEVINFO DeviceInfoSet, PSP_DEVINFO_DATA DeviceInfoData, const GUID* InterfaceClassGuid, DWORD MemberIndex, PSP_DEVICE_INTERFACE_DATA DeviceInterfaceData)
//{
//	bool result = SetupDiEnumDeviceInterfaces(DeviceInfoSet, DeviceInfoData, *InterfaceClassGuid, setupapid, DeviceInterfaceData);
//	return result;
//}

void SetWindowPosHook::ShowGuiStatus()
{
	int pos[2] = { posx, posy };
	ImGui::SliderInt2("Position", &pos[0], -5000, 5000);
	posx = pos[0];
	posy = pos[1];
	
	int size[2] = { width, height };
	ImGui::SliderInt2("Size", &size[0], 0, 5000);
	width = size[0];
	height = size[1];
}

void SetWindowPosHook::InstallImpl()
{
/*if (!HooksConfig::gConfigInitialized) {
	HooksConfig::gConfig = HooksConfig::ReadConfigFromIni();
	HooksConfig::gConfigInitialized = true;
}*/

hookInfo1 = std::get<1>(InstallNamedHook(L"user32", "SetWindowPos", Hook_SetWindowPos));
if (HooksConfig::gConfig.moveWindow) {
	hookInfo2 = std::get<1>(InstallNamedHook(L"user32", "MoveWindow", MoveWindow_Hook));
};
if (HooksConfig::gConfig.adjustWindowRect) {
	hookInfo3 = std::get<1>(InstallNamedHook(L"user32", "AdjustWindowRect", AdjustWindowRect_Hook));
};
if (HooksConfig::gConfig.adjustWindowRectEx) {
	hookInfo4 = std::get<1>(InstallNamedHook(L"user32", "AdjustWindowRectEx", AdjustWindowRectEx_Hook));
};
	
	hookInfo5 = std::get<1>(InstallNamedHook(L"user32", "GetMonitorInfo", GetMonitorInfo_hook));
	
}

void SetWindowPosHook::UninstallImpl()
{
UninstallHook(&hookInfo1);
UninstallHook(&hookInfo2);
UninstallHook(&hookInfo3);
UninstallHook(&hookInfo4);
UninstallHook(&hookInfo5);
}

}
