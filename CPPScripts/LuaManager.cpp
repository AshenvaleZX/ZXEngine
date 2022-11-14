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