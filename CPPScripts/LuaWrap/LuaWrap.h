#pragma once
#include "Lua_Debug.h"
#include "Lua_Resources.h"
#include "Lua_Time.h"
#include "Lua_GameLogic.h"
#include "Lua_Transform.h"
#include "Lua_GameObject.h"
#include "Lua_UITextRenderer.h"

static const luaL_Reg myLibs[] = {
    {"Debug", luaopen_Debug},
    {"Resources", luaopen_Resources},
    {"Time", luaopen_Time},
    {"GameLogic", luaopen_GameLogic},
    {"Transform", luaopen_Transform},
    {"GameObject", luaopen_GameObject},
    {"UITextRenderer", luaopen_UITextRenderer},
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