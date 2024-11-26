// dllmain.cpp : Defines the entry point for the DLL application.
#include <Windows.h>
#include <iostream>
#include <thread>

#include "console/console.h"

#include "hooks/hooks.h"
#include "utils/utils.h"

#include "MinHook.h"

#include "menu/menu.h"
#include "X4/lua.h"

DWORD WINAPI OnProcessAttach(LPVOID lpParam);
DWORD WINAPI OnProcessDetach(LPVOID lpParam);

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved) {
	if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
		DisableThreadLibraryCalls(hModule);

		HANDLE hHandle = CreateThread(NULL, 0, OnProcessAttach, hModule, 0, NULL);
		if (hHandle != NULL) {
			CloseHandle(hHandle);
		}
	}
	else if (ul_reason_for_call == DLL_PROCESS_DETACH && !lpReserved) {
		OnProcessDetach(NULL);
	}

	return TRUE;
}

DWORD WINAPI OnProcessAttach(LPVOID lpParam) {
	Console::Alloc();
	MH_Initialize();
	H::Init();

	if (!Lua::HookGame())
	{
		printf("Failed to hook game process, LuaExecutor.dll will unload");
		std::cin.get();
		Utils::UnloadDLL();
	}

	return 0;
}

DWORD WINAPI OnProcessDetach(LPVOID lpParam) {
	H::Free();
	MH_Uninitialize();

	Console::Free();

	return 0;
}
