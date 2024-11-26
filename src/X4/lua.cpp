#include "lua.h"

#include "MinHook.h"

namespace Lua
{
	static const struct luaL_Reg printlib[] = {
		{"print", LuaPrintToConsole},
		{NULL, NULL} /* end of array */
	};

	void* (*orig_GetComponentData)(void* L);
	void* hook_GetComponentData(void* L)
	{
		if (!Menu::x4_LuaState)
		{
			Menu::x4_LuaState = L;
			SetupFunctionPointers();
			printf("Found x4_LuaState: %p\n", Menu::x4_LuaState);
			if (MH_DisableHook((void*)Lua::GetComponentData_Address) == MH_OK)
			{
				printf("Disabled and GetComponentData() hook success\n");
			}
		}
		return orig_GetComponentData(L);
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
		GetComponentData_Address= (uintptr_t)GetModuleHandle(NULL) + 0x254B00;
		if (GetComponentData_Address)
		{
			if (MH_CreateHook((void*)GetComponentData_Address, (void*)hook_GetComponentData, (void**)&orig_GetComponentData) != MH_OK)
			{
				printf("Failed to create hook_GetComponentDatay\n");
				return false;
			}
			if (MH_EnableHook((void*)GetComponentData_Address) != MH_OK)
			{
				printf("Failed to enable hook_GetComponentData\n");
				return false;
			}
		}

		return true;
	}

	static void SetupFunctionPointers()
	{
		HMODULE luaDLL = GetModuleHandle("lua51_64.dll");
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