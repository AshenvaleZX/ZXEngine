#pragma once
#include "Lua_Debug.h"
#include "Lua_Resources.h"

static const luaL_Reg myLibs[] = {
  {"Debug", luaopen_Debug},
  {"Resources", luaopen_Resources},
  {NULL, NULL}
};

LUALIB_API void luaL_openMyLibs(lua_State* L) {
    const luaL_Reg* lib;
    /* "require" functions from 'loadedlibs' and set results to global table */
    for (lib = myLibs; lib->func; lib++) {
        luaL_requiref(L, lib->name, lib->func, 1);
        lua_pop(L, 1);  /* remove lib */
    }
}