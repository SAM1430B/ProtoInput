#pragma once
#include "Hook.h"
#include "InstallHooks.h"
#include "HooksConfig.h" // temp


namespace Proto
{

class SetWindowPosHook final : public Hook
{
private:
	HookInfo hookInfo1{};
	HookInfo hookInfo2{};
	HookInfo hookInfo3{};
	HookInfo hookInfo4{};
	HookInfo hookInfo5{};
		
public:
	static int width;
	static int height;
	static int posx;
	static int posy;

	static RECT rmonitor;
	static RECT rwork;
	
	const char* GetHookName() const override { return "Set Window Position"; }
	const char* GetHookDescription() const override
	{
		return
			"When the game tries to reposition/resize its game window, this hook forces it to a fixed position and size. ";
	}
	bool HasGuiStatus() const override { return true; }
	void ShowGuiStatus() override;
	void InstallImpl() override;
	void UninstallImpl() override;
};

}
