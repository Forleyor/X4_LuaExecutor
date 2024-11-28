#include "lua.h"

#include <psapi.h>

#include "MinHook.h"
#include "../mem/mem.h"

struct LuaEngine
{
	void* vTable;
	void* luaState;
	std::string luaString; // Used to find the correct state via unique string
};

namespace Lua
{
	static const struct luaL_Reg printlib[] = {
		{"print", LuaPrintToConsole},
		{NULL, NULL} /* end of array */
	};

	LuaEngine* (*orig_LuaInitialize)(LuaEngine *pThis, int a2);
	LuaEngine* hook_LuaInitialize(LuaEngine *pThis, int a2)
	{
		pThis = orig_LuaInitialize(pThis, a2);

		printf("%s\n", pThis->luaString.c_str());
		if (strcmp(pThis->luaString.c_str(), "ui/widget/Lua/") == 0)
		{
			Menu::x4_LuaState = pThis->luaState;
			SetupFunctionPointers();
			printf("Found x4_LuaState: %p\n", Menu::x4_LuaState);
		}

		return pThis;
	}

	static int LuaPrintToConsole(void* L)
	{
		int nargs = _lua_gettop(L);

		std::string output = "";

		for (int i = 1; i <= nargs; ++i)
		{
			const char *textResult = _lua_tolstring(L, i, nullptr);
			if (textResult)
				output = textResult;
			_lua_settop(L, -(nargs)-1);
		}

		Menu::console.SetText(Menu::console.GetText() + output);

		return 0;
	}

	bool HookGame()
	{
		MODULEINFO info;
		BOOL result = GetModuleInformation(GetCurrentProcess(), GetModuleHandle(nullptr), &info, sizeof(info));

		if (!result)
			printf("Failed to get module info for X4, Lua Executor will not work");
	
		LuaInitialize_Address = (uintptr_t)scan_idastyle(info.lpBaseOfDll, info.SizeOfImage, "48 89 5C 24 ? 57 48 83 EC 50 48 8B F9 48 8D 05");
		if (LuaInitialize_Address)
		{
			if (MH_CreateHook((void*)LuaInitialize_Address, (void*)hook_LuaInitialize, (void**)&orig_LuaInitialize) != MH_OK)
			{
				printf("Failed to create hook_LuaInitializey\n");
				return false;
			}
			if (MH_EnableHook((void*)LuaInitialize_Address) != MH_OK)
			{
				printf("Failed to enable hook_LuaInitialize\n");
				return false;
			}
		}

		return true;
	}

	static void SetupFunctionPointers()
	{
		HMODULE luaDLL = GetModuleHandle(L"lua51_64.dll");
		_lua_gettop = (__lua_gettop)GetProcAddress(luaDLL, "lua_gettop");
		_lua_settop = (__lua_settop)GetProcAddress(luaDLL, "lua_settop");
		_luaL_loadstring = (__luaL_loadstring)GetProcAddress(luaDLL, "luaL_loadstring");
		_lua_pcall = (__lua_pcall)GetProcAddress(luaDLL, "lua_pcall");
		_lua_getfield = (__lua_getfield)GetProcAddress(luaDLL, "lua_getfield");
		_luaL_register = (__luaL_register)GetProcAddress(luaDLL, "luaL_register");
		_lua_tolstring = (__lua_tolstring)GetProcAddress(luaDLL, "lua_tolstring");

		_lua_getfield(Menu::x4_LuaState, LUA_GLOBALSINDEX, "_G");
		_luaL_register(Menu::x4_LuaState, NULL, (void*)printlib);
	}
}