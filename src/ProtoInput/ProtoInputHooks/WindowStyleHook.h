#pragma once
#include "Hook.h"
#include "InstallHooks.h"
#include "HwndSelector.h"
#include "HooksConfig.h" // temp

namespace Proto
{

class WindowStyleHook final : public Hook
{
private:
	HookInfo hookInfoA{};
	HookInfo hookInfoW{};
	HookInfo hookInfoPtrA{};
	HookInfo hookInfoPtrW{};

public:

	const char* GetHookName() const override { return "Window Style"; }
	const char* GetHookDescription() const override
	{
		return
			"Hooks SetWindowLong and SetWindowLongPtr to prevent the game from re-adding a title bar. ";
	}
	bool HasGuiStatus() const override { return false; }

	//struct WindowMetrics {
	//	RECT windowRect;    // Full window rectangle
	//	RECT clientRect;    // Client area rectangle
	//	int borderWidth;    // Border width
	//	int titleHeight;    // Title bar height
	//};

	void InstallImpl() override;
	void UninstallImpl() override;
};

}
