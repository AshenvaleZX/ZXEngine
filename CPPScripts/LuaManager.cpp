#include "LuaManager.h"
#include "Resources.h"
#include "ProjectSetting.h"
#include "LuaWrap/LuaWrap.h"

namespace ZXEngine
{
	LuaManager* LuaManager::mInstance = nullptr;

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

	LuaManager::LuaManager()
	{
		// InitLuaState();
	}

	lua_State* LuaManager::GetState()
	{
		return L;
	}

	void LuaManager::InitLuaState()
	{
		L = luaL_newstate(); /* create state */
		luaL_openlibs(L);    /* 加载标准库 */
		luaL_openMyLibs(L);  /* 加载自定义库 */

		// 将内置Lua源码的路径加入package.path
		lua_getglobal(L, "package");
		lua_getfield(L, -1, "path");         /* 获取package.path */
		const char* cur_path = lua_tostring(L, -1);
		string new_path(cur_path);
		new_path += ";" + ProjectSetting::mBuiltInAssetsPath + "/Scripts/?.lua";
		lua_pop(L, 1);                       /* 弹出原来的package.path */
		lua_pushstring(L, new_path.c_str()); /* 压入新的package.path */
		lua_setfield(L, -2, "path");         /* 对package.path赋值 */
		lua_pop(L, 1);                       /* 弹出package库 */

		// 执行Lua启动脚本
		auto suc = luaL_dofile(L, Resources::GetAssetFullPath("Scripts/Init.lua", true).c_str());
		// 输出错误日志
		if (suc != LUA_OK)
			Debug::LogError(lua_tostring(L, -1));
	}

	void LuaManager::RestartLuaState()
	{
		if (L) lua_close(L);
		InitLuaState();
	}

	void LuaManager::Unref(int ref)
	{
		luaL_unref(L, LUA_REGISTRYINDEX, ref);
	}

	void LuaManager::CallFunction(int table, int func)
	{
		// 函数入栈
		lua_rawgeti(L, LUA_REGISTRYINDEX, func);
		// table入栈
		lua_rawgeti(L, LUA_REGISTRYINDEX, table);

		if (lua_isfunction(L, -2))
		{
			// 调用函数，1参数，0返回值
			if (lua_pcall(L, 1, 0, 0) != LUA_OK)
			{
				Debug::LogError(lua_tostring(L, -1));
			}
		}
		else
		{
			Debug::LogWarning("Called invalid function: " + (string)lua_tostring(L, -2));
		}
	}

	void LuaManager::CallFunction(const char* table, const char* func, const char* msg, bool self)
	{
		// 记录当前栈大小
		int stack_size = lua_gettop(L);
		// global table名入栈
		lua_getglobal(L, table);
		// 函数名入栈
		lua_pushstring(L, func);
		// 获取函数func
		int type = lua_gettable(L, -2);
		// 检查一下有没有这个函数
		if (type != LUA_TFUNCTION)
		{
			Debug::LogWarning("Called invalid global function: " + (string)func);
			return;
		}
		if (self)
		{
			// 把位于-2位置上的table复制一遍，重新入栈，作为调用func的self参数，对应lua那边的:调用
			lua_pushvalue(L, -2);
		}
		// 参数入栈
		lua_pushstring(L, msg);
		// 调用函数func，1或2个参数，0返回值
		if (lua_pcall(L, self?2:1, 0, 0) != LUA_OK)
		{
			// 调用失败打印日志
			Debug::LogError(lua_tostring(L, -1));
		}
		// 恢复栈大小(Pop掉这段代码在栈上产生的数据)
		lua_settop(L, stack_size);
	}

	void LuaManager::CallGlobalFunction(const char* func, const char* msg)
	{
		CallFunction(LUA_GNAME, func, msg, false);
	}
}