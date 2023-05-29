#include "SceneManager.h"
#include "Resources.h"
#include "ProjectSetting.h"
#include "RenderAPI.h"
#include "LuaManager.h"
#include "RenderPassManager.h"

namespace ZXEngine
{
	SceneManager* SceneManager::mInstance = nullptr;

	void SceneManager::Create()
	{
		mInstance = new SceneManager();
		mInstance->LoadScene(ProjectSetting::defaultScene);
	}

	SceneManager* SceneManager::GetInstance()
	{
		return mInstance;
	}

	Scene* SceneManager::GetScene(string name)
	{
		auto sceneInfo = GetSceneInfo(name);
		if (sceneInfo == nullptr)
		{
			return nullptr;
		}
		else
		{
			return sceneInfo->scene;
		}
	}

	void SceneManager::LoadScene(string path, bool switchNow)
	{
		// 目前多场景同时管理还有问题，主要体现在GameLogic和Light等组件上，会记录到一个全局列表里
		// 然后在遍历这种组件的时候，会遍历到不属于当前场景的对象，比如调用到不属于当前场景的Lua脚本
		// 所以暂时只支持单场景，切场景的时候先卸载原场景
		DeleteAllScene();
		string name = Resources::GetAssetName(path);

		SceneInfo* info = new SceneInfo();
		info->path = path;
		info->scene = new Scene(Resources::LoadScene(path));
		scenes.insert(pair<string, SceneInfo*>(name, info));

		if (switchNow)
			SwitchScene(name);
	}

	void SceneManager::SwitchScene(string name)
	{
		auto scene = GetSceneInfo(name);
		if (scene == nullptr)
		{
			Debug::LogError("Switch to invalid scene: " + name);
		}
		else
		{
			curScene = scene;
			ProjectSetting::renderPipelineType = curScene->scene->renderPipelineType;
			LuaManager::GetInstance()->RestartLuaState();
			RenderPassManager::GetInstance()->SetUpRenderPasses();
		}
	}

	void SceneManager::DeleteScene(string name)
	{
		map<string, SceneInfo*>::iterator iter = scenes.find(name);
		if (iter != scenes.end())
		{
			delete iter->second;
			scenes.erase(iter);
		}
		else
		{
			Debug::LogWarning("Attempt to delete a nonexistent scene: " + name);
		}
	}

	void SceneManager::DeleteAllScene()
	{
		for (auto& iter : scenes)
		{
			delete iter.second;
		}
		scenes.clear();
	}

	void SceneManager::ReloadScene()
	{
		// 先等待当前所有绘制结束，再卸载场景释放资源
		RenderAPI::GetInstance()->WaitForRenderFinish();
		delete curScene->scene;
		curScene->scene = new Scene(Resources::LoadScene(curScene->path));
		LuaManager::GetInstance()->RestartLuaState();
	}

	Scene* SceneManager::GetCurScene()
	{
		return curScene->scene;
	}

	SceneInfo* SceneManager::GetSceneInfo(string name)
	{
		map<string, SceneInfo*>::iterator iter = scenes.find(name);
		if (iter != scenes.end())
			return iter->second;
		else
			return nullptr;
	}
}