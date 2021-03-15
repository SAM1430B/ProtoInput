// dllmain.cpp : Defines the entry point for the DLL application.
#include <iostream>
#include <BlackBone/Process/Process.h>
#include <BlackBone/Patterns/PatternSearch.h>
#include <BlackBone/Process/RPC/RemoteFunction.hpp>
#include <BlackBone/Syscalls/Syscall.h>
#include <imgui.h>

#include <easyhook.h>


#include "Gui.h"
#include "RawInput.h"
#include "HookManager.h"
#include "MenuShortcut.h"
#include "protoloader.h"
#include "PipeCommunication.h"

HMODULE dll_hModule;

DWORD WINAPI GuiThread(LPVOID lpParameter)
{
    std::cout << "Starting gui thread\n";
    Proto::AddThreadToACL(GetCurrentThreadId());
    Proto::ShowGuiImpl();
   
    return 0;
}

DWORD WINAPI StartThread(LPVOID lpParameter)
{
    AllocConsole();
    FILE* f = new FILE();
    freopen_s(&f, "CONOUT$", "w", stdout);
    freopen_s(&f, "CONOUT$", "w", stderr);
    Proto::ConsoleHwnd = (intptr_t)FindWindowW(L"ConsoleWindowClass", NULL);
    
    std::cout << "Hooks DLL loaded\n";

	// Useful to add a pause if we need to attach a debugger
    // MessageBoxW(NULL, L"Press OK to start", L"", MB_OK);

    Proto::AddThreadToACL(GetCurrentThreadId());
	
    HANDLE hGuiThread = CreateThread(nullptr, 0,
                                  (LPTHREAD_START_ROUTINE)GuiThread, dll_hModule, CREATE_SUSPENDED, &Proto::GuiThreadID);
  
    Proto::RawInput::InitialiseRawInput();

    Proto::StartPipeCommunication();
	    
    ResumeThread(hGuiThread);

    if (hGuiThread != nullptr)
        CloseHandle(hGuiThread);
		
    std::cout << "Reached end of startup thread\n";
    	
    return 0;
}

// EasyHook entry point
extern "C" __declspec(dllexport) void __stdcall NativeInjectionEntryPoint(REMOTE_ENTRY_INFO * inRemoteInfo)
{
    //MessageBoxW(NULL, L"NativeInjectionEntryPoint", L"", MB_OK);

	
    printf("EasyHook NativeInjectionEntryPoint called\n");
	
    // //TODO: we might need to wait (mutex or sleep) for tasks to complete before waking up
    // std::cout << "Waking up original process (if necessary)\n" << std::flush;
    // RhWakeUpProcess();
}

EASYHOOK_BOOL_EXPORT EasyHookDllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved);

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
	// Needed for EasyHook to initialise some stuff
    EasyHookDllMain(hModule, ul_reason_for_call, lpReserved);
	
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    {
        dll_hModule = hModule;
    		
         HANDLE hThread = CreateThread(nullptr, 0,
                                      (LPTHREAD_START_ROUTINE)StartThread, hModule, 0, 0);
         if (hThread != nullptr)
             CloseHandle(hThread);
             		
        break;
    }
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

