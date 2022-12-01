#include "GameLogic.h"
#include "GameLogicManager.h"
#include "LuaManager.h"
#include "Resources.h"

namespace ZXEngine
{
	GameLogic::GameLogic()
	{
		GameLogicManager::GetInstance()->AddGameLogic(this);
	}

	GameLogic::~GameLogic()
	{
		GameLogicManager::GetInstance()->RemoveGameLogic(this);
	}

	ComponentType GameLogic::GetType()
	{
		return ComponentType::T_GameLogic;
	}

	void GameLogic::Start()
	{
		lua_State* L = LuaManager::GetInstance()->GetState();
		string p = "Scripts/" + luaName + ".lua";
		auto suc = luaL_dofile(L, Resources::GetAssetFullPath(p.c_str()).c_str());
		if (suc == LUA_OK)
		{
			// 这里dofile成功后，lua代码的最后一行是return table，此时栈顶是一个table
			// 把键值入栈
			lua_pushstring(L, "_ID");
			// 准备获取lua return的GameLogic._ID，因为刚压入了一个key，所以table现在在-2的位置
			lua_gettable(L, -2);
			// 这里真正的获取到GameLogic._ID
			luaID = (int)lua_tonumber(L, -1);
			// lua_tonumber不会自动出栈，手动pop一下，table回到-1位置
			lua_pop(L, 1);

			// 把"GameLogic"字符串入栈，准备作为Lua table访问this的字段名
			lua_pushstring(L, "GameLogic");
			// 新建一个GameLogic*的userdata，入栈，作为访问this的指针
			GameLogic** data = (GameLogic**)lua_newuserdata(L, sizeof(GameLogic*));
			// 把刚刚新建的指针指向this
			*data = this;

			// 获取this对应的metatable，入栈
			luaL_getmetatable(L, "ZXEngine.GameLogic");
			// 给this(现在在-2位置)设置meta table，同时meta table出栈
			lua_setmetatable(L, -2);

			// 此时-1位置是this，-2位置是"GameLogic"，-3位置是绑定的lua table
			// 设置table["GameLogic"] = this
			lua_settable(L, -3);

			CallLuaFunction("Start");
		}
		else
		{
			Debug::Log(lua_tostring(L, -1));
		}
	}

	void GameLogic::Update()
	{
		if (firstCall)
		{
			Start();
			firstCall = false;
		}

		CallLuaFunction("Update");
	}

	void GameLogic::CallLuaFunction(const char* func)
	{
		lua_State* L = LuaManager::GetInstance()->GetState();
		// 此时全局table AllGameLogic在-1位置
		lua_getglobal(L, "AllGameLogic");
		// 此时全局table AllGameLogic在-2位置
		lua_pushnumber(L, luaID);
		// 获取AllGameLogic[luaID]，此时table AllGameLogic[luaID]在-1
		lua_gettable(L, -2);
		// 压入函数名，此时table AllGameLogic[luaID]在-2
		lua_pushstring(L, func);
		// 获取AllGameLogic[luaID]的func函数
		int type = lua_gettable(L, -2);
		// 检查一下有没有这个函数
		if (type != LUA_TFUNCTION)
		{
			Debug::LogWarning("Called invalid GameLogic function: " + (string)func);
			return;
		}
		// 把位于-2位置上的table复制一遍，重新入栈，作为调用func的参数(就是self，lua那边函数定义写的:)
		lua_pushvalue(L, -2);
		// 调用AllGameLogic[luaID]的func函数，1参数，0返回值
		if (lua_pcall(L, 1, 0, 0) != LUA_OK)
		{
			// 调用失败打印日志
			Debug::LogError(lua_tostring(L, -1));
		}
	}
}