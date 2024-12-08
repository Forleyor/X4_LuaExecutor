#include "lua.h"

#include <psapi.h>

#include "MinHook.h"
#include "defs.h"
#include "../utils/mem.h"


namespace Lua
{
	struct LuaEngine
	{
		void* vTable;
		void* luaState;
		std::string luaString; // Used to find the correct state via unique string
	};

	LuaEngine* (*orig_LuaInitialize)(LuaEngine *pThis, int a2);
	LuaEngine* hook_LuaInitialize(LuaEngine *pThis, int a2)
	{
		pThis = orig_LuaInitialize(pThis, a2);

		if (strcmp(pThis->luaString.c_str(), "ui/widget/Lua/") == 0)
		{
			if (widget_luastate == nullptr)
			{
				SetupFunctionPointers();

				widget_luastate = pThis->luaState;
				printf("widget_luastate: %p\n", widget_luastate);
			}
			else if (widget_luastate != pThis->luaState)
			{
				widget_luastate = pThis->luaState;
				printf("Updated widget_luastate: %p\n", widget_luastate);
			}

			DefineLuaFunctions();
			_lua_pushcclosure(widget_luastate, LuaPrintToConsole, 0);
			_lua_setfield(widget_luastate, LUA_GLOBALSINDEX, "print");
			printf("\n==============================================================\n\n");
		}

		return pThis;
	}

	int (*orig_LuaGettop)(void* L);
	int hook_LuaGettop(void* L)
	{
		int result = orig_LuaGettop(L);
		if (!code.empty() && widget_luastate == L)
		{
			if (_luaL_loadstring(widget_luastate, code.c_str()) == LUA_OK)
			{
				if (code.find("PrintTable") != std::string::npos)
					Menu::console.GetText().clear();
				if (Menu::autoExectute)
					Menu::console.GetText().clear();;

				code.clear();
				if (_lua_pcall(widget_luastate, 0, LUA_MULTRET, 0) != LUA_OK)
				{
					Menu::console.SetText(_lua_tolstring(widget_luastate, -1, 0));
				}
			}
			else
			{
				code.clear();
				Menu::console.SetText(_lua_tolstring(widget_luastate, -1, 0));
			}
		}
		return result;
	}

	static int LuaPrintToConsole(void* L)
	{
		int nargs = _lua_gettop(L);

		std::string output = "";

		for (int i = 1; i <= nargs; ++i)
		{
			const char* textResult = _lua_tolstring(L, i, nullptr);
			if (textResult)
				output = textResult;
			lua_pop(widget_luastate, nargs);
		}

		Menu::console.SetText(Menu::console.GetText() + output);

		return 0;
	}

	bool HookGame()
	{
		MODULEINFO info;
		BOOL result = GetModuleInformation(GetCurrentProcess(), GetModuleHandle(nullptr), &info, sizeof(info));

		if (!result)
		{
			printf("Failed to get module info for X4, Lua Executor will not work\n");
			return false;
		}

		uintptr_t LuaInitialize_Address = (uintptr_t)scan_idastyle(info.lpBaseOfDll, info.SizeOfImage, "48 89 5C 24 ? 57 48 83 EC 50 48 8B F9 48 8D 05");
		if (LuaInitialize_Address)
		{
			if (MH_CreateHook((void*)LuaInitialize_Address, (void*)hook_LuaInitialize, (void**)&orig_LuaInitialize) != MH_OK)
				return false;
			if (MH_EnableHook((void*)LuaInitialize_Address) != MH_OK)
				return false;
			printf("Hooked LuaInitialize_Address\n");
		}

		uintptr_t luaGettop_Address = (uintptr_t)GetProcAddress(GetModuleHandleA("lua51_64.dll"), "lua_gettop");
		if (luaGettop_Address)
		{
			if (MH_CreateHook((void*)luaGettop_Address, (void*)hook_LuaGettop, (void**)&orig_LuaGettop) != MH_OK)
				return false;
			if (MH_EnableHook((void*)luaGettop_Address) != MH_OK)
				return false;
			printf("Hooked lua_gettop\n");
		}

		return true;
	}

	static void SetupFunctionPointers()
	{
		HMODULE luaDLL = GetModuleHandleA("lua51_64.dll");
		_lua_gettop = (__lua_gettop)GetProcAddress(luaDLL, "lua_gettop");
		_lua_settop = (__lua_settop)GetProcAddress(luaDLL, "lua_settop");
		_lua_newthread = (__lua_newthread)GetProcAddress(luaDLL, "lua_newthread");
		_luaL_loadstring = (__luaL_loadstring)GetProcAddress(luaDLL, "luaL_loadstring");
		_lua_pcall = (__lua_pcall)GetProcAddress(luaDLL, "lua_pcall");
		_lua_getfield = (__lua_getfield)GetProcAddress(luaDLL, "lua_getfield");
		_luaL_register = (__luaL_register)GetProcAddress(luaDLL, "luaL_register");
		_lua_tolstring = (__lua_tolstring)GetProcAddress(luaDLL, "lua_tolstring");
		_lua_pushcclosure = (__lua_pushcclosure)GetProcAddress(luaDLL, "lua_pushcclosure");
		_lua_setfield = (__lua_setfield)GetProcAddress(luaDLL, "lua_setfield");

		printf("SetupFunctionPointers done\n");
	}

	static void DefineLuaFunctions()
	{
		const char* LogTable = R"(
			function LogTable(itable)
				local line

				for k,v in pairs(itable) do
					line = "["..k.."] = "..tostring(v).." ("..type(v)..")\n"
					if #line + #str >= 8192 then
						DebugError(str)
						str = line
					else
						str = str .. line
					end
				end
				DebugError(str)
			end
		)";

		const char* PrintTable = R"(
			function PrintTable(itable, name)
				if not name then name = "" end
				-- Construct a string with newlines between table entries.
				-- Start with header.
				local str = "Table "..name.." contents:\n"
				local line

				for k,v in pairs(itable) do
					line = "["..k.."] = "..tostring(v).." ("..type(v)..")\n"
					-- If this line will put the str over 8192, do an early str dump
					-- first.
					if #line + #str >= 8192 then
						print(str)
						-- Restart the str.
						str = line
					else
						-- Append to running str.
						str = str .. line
					end
				end
				print(str)
			end
		)";

		printf("\nDefining lua helper functions\n");
		if (luaL_dostring(widget_luastate, LogTable) == LUA_OK)
			printf("Defined function LogTable(itable, name) -> Logs table to X4 Debug Log \n");
		if (luaL_dostring(widget_luastate, PrintTable) == LUA_OK)
			printf("Defined function PrintTable(itable, name) -> Prints table to Lua Executors console\n");
	}
}