#pragma once
#include "Lua_Debug.h"
#include "Lua_Resources.h"
#include "Lua_Time.h"
#include "Lua_Transform.h"
#include "Lua_GameObject.h"
#include "Lua_UITextRenderer.h"
#include "Lua_InputManager.h"
#include "Lua_Animator.h"
#include "Lua_Rigidbody.h"
#include "Lua_Camera.h"

static const luaL_Reg myLibs[] = 
{
    { "Debug",          luaopen_Debug          },
    { "Resources",      luaopen_Resources      },
    { "Time",           luaopen_Time           },
    { "Transform",      luaopen_Transform      },
    { "GameObject",     luaopen_GameObject     },
    { "UITextRenderer", luaopen_UITextRenderer },
    { "InputManager",   luaopen_InputManager   },
    { "Animator",       luaopen_Animator       },
    { "Rigidbody",      luaopen_RigidBody      },
    { "Camera",         luaopen_Camera         },
    { NULL, NULL }
};

LUALIB_API void luaL_openMyLibs(lua_State* L) 
{
    const luaL_Reg* lib;
    for (lib = myLibs; lib->func; lib++) 
    {
        luaL_requiref(L, lib->name, lib->func, 1);
        // remove lib
        lua_pop(L, 1);
    }
}