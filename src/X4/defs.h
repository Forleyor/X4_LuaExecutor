#pragma once

/* pseudo-indices */
#define LUA_REGISTRYINDEX       -10000
#define LUA_ENVIRONINDEX        -10001
#define LUA_GLOBALSINDEX        -10002

/* thread status */
#define LUA_OK                  0
#define LUA_YIELD               1
#define LUA_ERRRUN              2
#define LUA_ERRSYNTAX           3
#define LUA_ERRMEM              4
#define LUA_ERRERR              5

/* lua types */
#define LUA_TNIL		        0
#define LUA_TBOOLEAN		    1
#define LUA_TLIGHTUSERDATA	    2
#define LUA_TNUMBER		        3
#define LUA_TSTRING		        4
#define LUA_TTABLE		        5
#define LUA_TFUNCTION		    6
#define LUA_TUSERDATA		    7
#define LUA_TTHREAD		        8

#define LUA_MULTRET -1

#define luaL_dostring(L, s)     (_luaL_loadstring(L, s) || _lua_pcall(L, 0, LUA_MULTRET, 0))
#define lua_pop(L,n)            _lua_settop(L, -(n)-1)
#define lua_getglobal(L,s)      _lua_getfield(L, LUA_GLOBALSINDEX, s)
#define lua_isfunction(L,n)	    (_lua_type(L, (n)) == LUA_TFUNCTION)
#define lua_istable(L,n)	    (_lua_type(L, (n)) == LUA_TTABLE)