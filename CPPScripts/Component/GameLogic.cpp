#include "GameLogic.h"
#include "../GameLogicManager.h"
#include "../LuaManager.h"
#include "../Resources.h"

namespace ZXEngine
{
	ComponentType GameLogic::GetType()
	{
		return ComponentType::GameLogic;
	}

	GameLogic::GameLogic()
	{
		GameLogicManager::GetInstance()->AddGameLogic(this);
	}

	GameLogic::~GameLogic()
	{
		GameLogicManager::GetInstance()->RemoveGameLogic(this);
	}

	ComponentType GameLogic::GetInsType()
	{
		return ComponentType::GameLogic;
	}

	void GameLogic::Awake()
	{
		lua_State* L = LuaManager::GetInstance()->GetState();
		// 记录当前栈大小
		int stack_size = lua_gettop(L);
		// 加载绑定的lua代码
		auto suc = luaL_dofile(L, luaFullPath.c_str());
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

			// 把"GameLogic"字符串入栈，准备作为Lua table访问this->gameObject的字段名
			lua_pushstring(L, "gameObject");
			// 新建一个GameObject*的userdata，入栈，作为访问this->gameObject的指针
			GameObject** data = (GameObject**)lua_newuserdata(L, sizeof(GameObject*));
			// 把刚刚新建的指针指向this->gameObject
			*data = this->gameObject;

			// 获取GameObject对应的metatable，入栈
			luaL_getmetatable(L, "ZXEngine.GameObject");
			// 给this->gameObject(现在在-2位置)设置meta table，同时meta table出栈
			lua_setmetatable(L, -2);

			// 此时-1位置是this->gameObject，-2位置是"GameObject"，-3位置是绑定的lua table
			// 设置table["gameObject"] = this->gameObject
			lua_settable(L, -3);

			CallLuaFunction("Awake");
		}
		else
		{
			Debug::Log(lua_tostring(L, -1));
		}
		// 恢复栈大小(Pop掉这段代码在栈上产生的数据)
		lua_settop(L, stack_size);

		mIsAwake = true;
	}

	void GameLogic::Start()
	{
		CallLuaFunction("Start");
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
		// 记录当前栈大小
		int stack_size = lua_gettop(L);
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
			// 恢复栈大小(Pop掉这段代码在栈上产生的数据)
			lua_settop(L, stack_size);

			if (func != "Awake" && func != "Start" && func != "Update")
				Debug::LogWarning("GameLogic trying to call a lua function that doesn't exist: " + (string)func);

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
		// 恢复栈大小(Pop掉这段代码在栈上产生的数据)
		lua_settop(L, stack_size);
	}
}