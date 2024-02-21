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
		// ��¼��ǰջ��С
		int stack_size = lua_gettop(L);
		// ���ذ󶨵�lua����
		auto suc = luaL_dofile(L, luaFullPath.c_str());
		if (suc == LUA_OK)
		{
			// ����dofile�ɹ���lua��������һ����return table����ʱջ����һ��table
			// �Ѽ�ֵ��ջ
			lua_pushstring(L, "_ID");
			// ׼����ȡlua return��GameLogic._ID����Ϊ��ѹ����һ��key������table������-2��λ��
			lua_gettable(L, -2);
			// ���������Ļ�ȡ��GameLogic._ID
			luaID = (int)lua_tonumber(L, -1);
			// lua_tonumber�����Զ���ջ���ֶ�popһ�£�table�ص�-1λ��
			lua_pop(L, 1);

			// ��"GameLogic"�ַ�����ջ��׼����ΪLua table����this->gameObject���ֶ���
			lua_pushstring(L, "gameObject");
			// �½�һ��GameObject*��userdata����ջ����Ϊ����this->gameObject��ָ��
			GameObject** data = (GameObject**)lua_newuserdata(L, sizeof(GameObject*));
			// �Ѹո��½���ָ��ָ��this->gameObject
			*data = this->gameObject;

			// ��ȡGameObject��Ӧ��metatable����ջ
			luaL_getmetatable(L, "ZXEngine.GameObject");
			// ��this->gameObject(������-2λ��)����meta table��ͬʱmeta table��ջ
			lua_setmetatable(L, -2);

			// ��ʱ-1λ����this->gameObject��-2λ����"GameObject"��-3λ���ǰ󶨵�lua table
			// ����table["gameObject"] = this->gameObject
			lua_settable(L, -3);

			CallLuaFunction("Awake");
		}
		else
		{
			Debug::Log(lua_tostring(L, -1));
		}
		// �ָ�ջ��С(Pop����δ�����ջ�ϲ���������)
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
		// ��¼��ǰջ��С
		int stack_size = lua_gettop(L);
		// ��ʱȫ��table AllGameLogic��-1λ��
		lua_getglobal(L, "AllGameLogic");
		// ��ʱȫ��table AllGameLogic��-2λ��
		lua_pushnumber(L, luaID);
		// ��ȡAllGameLogic[luaID]����ʱtable AllGameLogic[luaID]��-1
		lua_gettable(L, -2);
		// ѹ�뺯��������ʱtable AllGameLogic[luaID]��-2
		lua_pushstring(L, func);
		// ��ȡAllGameLogic[luaID]��func����
		int type = lua_gettable(L, -2);
		// ���һ����û���������
		if (type != LUA_TFUNCTION)
		{
			// �ָ�ջ��С(Pop����δ�����ջ�ϲ���������)
			lua_settop(L, stack_size);

			if (func != "Awake" && func != "Start" && func != "Update")
				Debug::LogWarning("GameLogic trying to call a lua function that doesn't exist: " + (string)func);

			return;
		}
		// ��λ��-2λ���ϵ�table����һ�飬������ջ����Ϊ����func�Ĳ���(����self��lua�Ǳߺ�������д��:)
		lua_pushvalue(L, -2);
		// ����AllGameLogic[luaID]��func������1������0����ֵ
		if (lua_pcall(L, 1, 0, 0) != LUA_OK)
		{
			// ����ʧ�ܴ�ӡ��־
			Debug::LogError(lua_tostring(L, -1));
		}
		// �ָ�ջ��С(Pop����δ�����ջ�ϲ���������)
		lua_settop(L, stack_size);
	}
}