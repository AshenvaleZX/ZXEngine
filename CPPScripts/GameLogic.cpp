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

	void GameLogic::Start()
	{
		lua_State* L = LuaManager::GetInstance()->GetState();
		string p = "Scripts/" + luaName + ".lua";
		auto suc = luaL_dofile(L, Resources::GetAssetFullPath(p.c_str()).c_str());
		if (suc == LUA_OK)
		{
			// ����dofile�ɹ���lua��������һ����return table����ʱջ����һ��table
			// �Ѽ�ֵ��ջ
			lua_pushstring(L, "_ID");
			// ׼����ȡlua return��GameLogic._ID����Ϊ��ѹ����һ��key������table������-2��λ��
			lua_gettable(L, -2);
			// �����GameLogic._ID��ջ�������أ������Ļ�ȡ��GameLogic._ID
			luaID = (int)lua_tonumber(L, -1);

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
			Debug::LogWarning("Called invalid GameLogic function: " + (string)func);
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
	}
}