#pragma once

#include "defs.h"
#include "menu/menu.h"

typedef int (*__lua_gettop)(void* L);
inline __lua_gettop _lua_gettop;

typedef void (*__lua_settop)(void* L, int idx);
inline __lua_settop _lua_settop;

typedef void* (*__lua_newthread)(void* L);
inline __lua_newthread _lua_newthread;

typedef int (*__luaL_loadstring)(void* L, const char* s);
inline __luaL_loadstring _luaL_loadstring;

typedef int (*__lua_pcall)(void* L, int nargs, int nresults, int errfunc);
inline __lua_pcall _lua_pcall;

typedef int (*__lua_getfield)(void* L, int idx, const char* k);
inline __lua_getfield _lua_getfield;

typedef void (*__luaL_register)(void* L, const char* libname, void* l);
inline __luaL_register _luaL_register;

typedef const char* (*__lua_tolstring)(void* L, int idx, size_t* len);
inline __lua_tolstring _lua_tolstring;

typedef void (*__lua_pushcclosure)(void* L, void* fn, int n);
inline __lua_pushcclosure _lua_pushcclosure;

typedef void (*__lua_setfield)(void* L, int idx, const char* k);
inline __lua_setfield _lua_setfield;

typedef void (*__lua_pushvalue)(void* L, int idx);
inline __lua_pushvalue _lua_pushvalue;

typedef int (*__lua_type)(void* L, int idx);
inline __lua_type _lua_type;

typedef void (*__lua_pushstring)(void* L, const char* s);
inline __lua_pushstring _lua_pushstring;

typedef void (*__lua_gettable)(void* L, int idx);
inline __lua_gettable _lua_gettable;

namespace Lua
{
    static int LuaPrintToConsole(void* L);
    bool HookGame();
    void SetupFunctionPointers(HMODULE luaDLL);
    void DefineLuaFunctions();

    inline std::string code;
    inline std::string currentTableData;
    static void* widget_luastate;
}