#pragma once
#include "../Component/Physics/ZRigidBody.h"

extern "C"
{
#include "../External/Lua/lua.h"
#include "../External/Lua/lualib.h"
#include "../External/Lua/lauxlib.h"
}

static int AddForce(lua_State* L)
{
	int argc = lua_gettop(L);
	if (argc == 2)
	{
		ZXEngine::ZRigidBody** rigidBody = (ZXEngine::ZRigidBody**)luaL_checkudata(L, -2, "ZXEngine.RigidBody");
		lua_getfield(L, -1, "x");
		float x = (float)luaL_checknumber(L, -1);
		lua_pop(L, 1);
		lua_getfield(L, -1, "y");
		float y = (float)luaL_checknumber(L, -1);
		lua_pop(L, 1);
		lua_getfield(L, -1, "z");
		float z = (float)luaL_checknumber(L, -1);
		lua_pop(L, 1);
		(*rigidBody)->AddForce(ZXEngine::Vector3(x, y, z));
	}
	else if (argc == 4)
	{
		ZXEngine::ZRigidBody** rigidBody = (ZXEngine::ZRigidBody**)luaL_checkudata(L, -4, "ZXEngine.RigidBody");
		float x = (float)luaL_checknumber(L, -3);
		float y = (float)luaL_checknumber(L, -2);
		float z = (float)luaL_checknumber(L, -1);
		(*rigidBody)->AddForce(ZXEngine::Vector3(x, y, z));
	}
	else
	{
		ZXEngine::Debug::LogError("Invalid argument count for AddForce");
	}
	return 0;
}

static int RigidBodyGetPosition(lua_State* L)
{
	ZXEngine::ZRigidBody** rigidBody = (ZXEngine::ZRigidBody**)luaL_checkudata(L, -1, "ZXEngine.RigidBody");

	ZXEngine::Vector3 position = (*rigidBody)->GetPosition();
	lua_newtable(L);
	lua_pushnumber(L, position.x);
	lua_setfield(L, -2, "x");
	lua_pushnumber(L, position.y);
	lua_setfield(L, -2, "y");
	lua_pushnumber(L, position.z);
	lua_setfield(L, -2, "z");

	return 1;
}

static int RigidBodySetPosition(lua_State* L)
{
	int argc = lua_gettop(L);
	if (argc == 2)
	{
		ZXEngine::ZRigidBody** rigidBody = (ZXEngine::ZRigidBody**)luaL_checkudata(L, -2, "ZXEngine.RigidBody");
		lua_getfield(L, -1, "x");
		float x = (float)luaL_checknumber(L, -1);
		lua_pop(L, 1);
		lua_getfield(L, -1, "y");
		float y = (float)luaL_checknumber(L, -1);
		lua_pop(L, 1);
		lua_getfield(L, -1, "z");
		float z = (float)luaL_checknumber(L, -1);
		lua_pop(L, 1);
		(*rigidBody)->SetPosition(ZXEngine::Vector3(x, y, z));
	}
	else if (argc == 4)
	{
		ZXEngine::ZRigidBody** rigidBody = (ZXEngine::ZRigidBody**)luaL_checkudata(L, -4, "ZXEngine.RigidBody");
		float x = (float)luaL_checknumber(L, -3);
		float y = (float)luaL_checknumber(L, -2);
		float z = (float)luaL_checknumber(L, -1);
		(*rigidBody)->SetPosition(ZXEngine::Vector3(x, y, z));
	}
	else
	{
		ZXEngine::Debug::LogError("Invalid argument count for SetPosition");
	}
	return 0;
}

static int GetVelocity(lua_State* L)
{
	ZXEngine::ZRigidBody** rigidBody = (ZXEngine::ZRigidBody**)luaL_checkudata(L, -1, "ZXEngine.RigidBody");

	ZXEngine::Vector3 velocity = (*rigidBody)->GetVelocity();
	lua_newtable(L);
	lua_pushnumber(L, velocity.x);
	lua_setfield(L, -2, "x");
	lua_pushnumber(L, velocity.y);
	lua_setfield(L, -2, "y");
	lua_pushnumber(L, velocity.z);
	lua_setfield(L, -2, "z");

	return 1;
}

static int SetVelocity(lua_State* L)
{
	int argc = lua_gettop(L);
	if (argc == 2)
	{
		ZXEngine::ZRigidBody** rigidBody = (ZXEngine::ZRigidBody**)luaL_checkudata(L, -2, "ZXEngine.RigidBody");
		lua_getfield(L, -1, "x");
		float x = (float)luaL_checknumber(L, -1);
		lua_pop(L, 1);
		lua_getfield(L, -1, "y");
		float y = (float)luaL_checknumber(L, -1);
		lua_pop(L, 1);
		lua_getfield(L, -1, "z");
		float z = (float)luaL_checknumber(L, -1);
		lua_pop(L, 1);
		(*rigidBody)->SetVelocity(ZXEngine::Vector3(x, y, z));
	}
	else if (argc == 4)
	{
		ZXEngine::ZRigidBody** rigidBody = (ZXEngine::ZRigidBody**)luaL_checkudata(L, -4, "ZXEngine.RigidBody");
		float x = (float)luaL_checknumber(L, -3);
		float y = (float)luaL_checknumber(L, -2);
		float z = (float)luaL_checknumber(L, -1);
		(*rigidBody)->SetVelocity(ZXEngine::Vector3(x, y, z));
	}
	else
	{
		ZXEngine::Debug::LogError("Invalid argument count for SetVelocity");
	}
	return 0;
}

static const struct luaL_Reg RigidBody_Funcs[] = {
	{ NULL, NULL }
};

static const struct luaL_Reg RigidBody_Funcs_Meta[] = {
	{ "AddForce", AddForce },
	{ "GetPosition", RigidBodyGetPosition },
	{ "SetPosition", RigidBodySetPosition },
	{ "GetVelocity", GetVelocity },
	{ "SetVelocity", SetVelocity },
	{ NULL, NULL }
};

LUAMOD_API int luaopen_RigidBody(lua_State* L)
{
	luaL_newmetatable(L, "ZXEngine.RigidBody");
	lua_pushvalue(L, -1);
	lua_setfield(L, -2, "__index");
	luaL_setfuncs(L, RigidBody_Funcs_Meta, 0);

	luaL_newlib(L, RigidBody_Funcs);
	return 1;
}