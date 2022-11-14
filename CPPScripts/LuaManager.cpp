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
		luaL_openMyLibs(L); /* �����Զ���� */

		// ִ��Lua�����ű�
		auto suc = luaL_dofile(L, Resources::GetAssetFullPath("Scripts/Init.lua").c_str());
		// ���������־
		if (suc == 1)
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
}