#pragma once

#include "defs.h"
#include "../menu/menu.h"

typedef int (*__lua_gettop)(void* L);
typedef void (*__lua_settop)(void* L, int idx);
typedef void* (*__lua_newthread)(void* L);
typedef int (*__luaL_loadstring)(void* L, const char* s);
typedef int (*__lua_pcall)(void* L, int nargs, int nresults, int errfunc);
typedef void (*__lua_getfield)(void* L, int idx, const char* k);
typedef void (*__luaL_register)(void* L, const char* libname, void* l);
typedef const char* (*__lua_tolstring)(void* L, int idx, size_t* len);
inline __lua_gettop _lua_gettop;
inline __lua_settop _lua_settop;
inline __luaL_loadstring _luaL_loadstring;
inline __lua_pcall _lua_pcall;
inline __lua_getfield _lua_getfield;
inline __luaL_register _luaL_register;
inline __lua_tolstring _lua_tolstring;

typedef struct luaL_Reg {
	const char* name;
	void* func;
} luaL_Reg;

namespace Lua
{
	static int LuaPrintToConsole(void* L);
	bool HookGame();
	void SetupFunctionPointers();

	static uintptr_t LuaInitialize_Address;
}