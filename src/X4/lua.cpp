#include "lua.h"

#include <iostream>
#include <fstream>
#include <filesystem>
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
        char* luaString; // Used to find the correct state via unique string
    };

    void doLuaExector(void *L)
    {
        if (_luaL_loadstring(widget_luastate, code.c_str()) == LUA_OK)
        {
            if (code.find("PrintTable") != std::string::npos)
                Menu::console.SetText("");
            if (Menu::autoExectute)
                Menu::console.SetText("");;

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

    void doTableViewer(void* L)
    {
        lua_getglobal(L, "GetTableData");
        if (lua_isfunction(L, -1))
        {
            _lua_pushvalue(L, LUA_GLOBALSINDEX);
            if (lua_istable(L, -1))
            {
                _lua_pcall(L, 1, 1, 0);
                std::string result = _lua_tolstring(L, -1, nullptr);
                Menu::ParseTable(result, true);
            }
        }
    }

    LuaEngine* (*orig_LuaInitialize)(LuaEngine *pThis, int a2);
    LuaEngine* hook_LuaInitialize(LuaEngine *pThis, int a2)
    {
        pThis = orig_LuaInitialize(pThis, a2);

        if (strcmp(pThis->luaString, "ui/widget/Lua/") == 0)
        {
            if (widget_luastate == nullptr)
            {
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
        }

        return pThis;
    }

    int (*orig_LuaGettop)(void* L);
    int hook_LuaGettop(void* L)
    {
        int result = orig_LuaGettop(L);
        if (widget_luastate == L)
        {
            switch (Menu::mode)
            {
            case Menu::Mode::LuaExecutor:
                if (!Lua::code.empty())
                    doLuaExector(L);
                break;
            case Menu::Mode::TableViewer:
                if (Menu::processTable)
                    doTableViewer(L);
                break;
            default:
                break;
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

        if (Menu::console.GetText() == "\n")
            Menu::console.SetText(output);
        else
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

        HMODULE luaDLL = GetModuleHandleA("lua51_64.dll");
        if (luaDLL)
        {
            uintptr_t luaGettop_Address = (uintptr_t)GetProcAddress(luaDLL, "lua_gettop");
            if (luaGettop_Address)
            {
                if (MH_CreateHook((void*)luaGettop_Address, (void*)hook_LuaGettop, (void**)&orig_LuaGettop) != MH_OK)
                    return false;
                if (MH_EnableHook((void*)luaGettop_Address) != MH_OK)
                    return false;
                printf("Hooked lua_gettop\n");
            }

            SetupFunctionPointers(luaDLL);
        }

        return true;
    }

    static void SetupFunctionPointers(HMODULE luaDLL)
    {
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
        _lua_pushvalue = (__lua_pushvalue)GetProcAddress(luaDLL, "lua_pushvalue");
        _lua_type = (__lua_type)GetProcAddress(luaDLL, "lua_type");
        _lua_pushstring = (__lua_pushstring)GetProcAddress(luaDLL, "lua_pushstring");
        _lua_gettable = (__lua_gettable)GetProcAddress(luaDLL, "lua_gettable");

        printf("SetupFunctionPointers done\n");
    }

    static void DefineLuaFunctions()
    {
        std::filesystem::path scriptPath = std::filesystem::current_path() / "plugins" / "LuaExecutor" / "Scripts" / "Utils.lua";
        std::ifstream luaFile(scriptPath, std::ios::in);
        if (luaFile.fail())
        {
            printf("Failed to open %s\nPredefined functions wont exist.\n", scriptPath.string().c_str());
            return;
        }

        std::string luaCode;
        std::string line;
        while (getline(luaFile, line))
        {
            luaCode += line;
            luaCode += "\n";
        }

        luaFile.close();

        printf("\nDefining lua helper functions\n");
        if (luaL_dostring(widget_luastate, luaCode.c_str()) == LUA_OK)
            printf("Defined functions from Utils.lua successfully\n");
        else
            printf("Defining functions from Utils.lua failed:\n%s\n", _lua_tolstring(widget_luastate, -1, 0));

        const char* GetTableData = R"(
            function GetTableData(itable)
                local str = ""

                for k,v in pairs(itable) do
                    line = k .. " " .. tostring(v) .. "\n"
                    str = str .. line
                end
                return str
            end
        )";

        printf("\nDefining GetTableData\n");
        if (luaL_dostring(widget_luastate, GetTableData) == LUA_OK)
            printf("Defined functions from GetTableData successfully\n");
        else
            printf("Defining functions from GetTableData failed:\n%s\n", _lua_tolstring(widget_luastate, -1, 0));
    }
}