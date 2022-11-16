#include "LuaManager.h"
#include "Resources.h"
#include "LuaWrap/LuaWrap.h"

namespace ZXEngine
{
	LuaManager* LuaManager::mInstance = nullptr;

	LuaManager::LuaManager()
	{
		L = luaL_newstate();  /* create state */
		luaL_openlibs(L);  /* open standard libraries */
		luaL_openMyLibs(L); /* 加载自定义库 */

		// 执行Lua启动脚本
		auto suc = luaL_dofile(L, Resources::GetAssetFullPath("Scripts/Init.lua").c_str());
		// 输出错误日志
		if (suc != LUA_OK)
			Debug::Log(lua_tostring(L, -1));
	}

	void LuaManager::Create()
	{
		mInstance = new LuaManager();
	}

	LuaManager* LuaManager::GetInstance()
	{
		return mInstance;
	}

	void LuaManager::PrintTable(lua_State* state, int tableIndex, const char* msg)
	{
		Debug::Log(msg);
		/* table is in the stack at index 't' */
		int top = lua_gettop(state);
		lua_pushnil(state);  /* first key */
		while (lua_next(state, tableIndex) != 0) 
		{
			/* uses 'key' (at index -2) and 'value' (at index -1) */
			std::stringstream ss;
			ss << "  " << lua_typename(state, lua_type(state, -2)) << " " << lua_tostring(state, -2) << " " << lua_typename(state, lua_type(state, -1));
			Debug::Log(ss.str());
			/* removes 'value'; keeps 'key' for next iteration */
			lua_pop(state, 1);
		}
		assert(lua_gettop(state) == top);
	}

	void LuaManager::PrintLuaState(lua_State* state, const char* msg)
	{
		Debug::Log(msg);
		int top = lua_gettop(state);
		for (int i = 1; i <= top; ++i) 
		{
			int t = lua_type(state, i);
			switch (t) 
			{
			case LUA_TSTRING:
				Debug::Log(lua_tostring(state, i));
				break;
			case LUA_TBOOLEAN:
				Debug::Log(lua_toboolean(state, i) ? "true" : "false");
				break;
			case LUA_TNUMBER:
				Debug::Log(lua_tostring(state, i));
				break;
			case LUA_TTABLE:
				PrintTable(state, i, "table: ");
				break;
			case LUA_TFUNCTION:
				Debug::Log("function");
				break;
			case LUA_TUSERDATA:
				Debug::Log("userdata");
				break;
			case LUA_TTHREAD:
				Debug::Log("thread");
				break;
			case LUA_TLIGHTUSERDATA:
				Debug::Log("light userdata");
				break;
			case LUA_TNIL:
				Debug::Log("nil");
				break;
			}
		}
	}

	lua_State* LuaManager::GetState()
	{
		return L;
	}
}