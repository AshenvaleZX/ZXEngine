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
		// Ŀǰ�ೡ��ͬʱ���������⣬��Ҫ������GameLogic��Light������ϣ����¼��һ��ȫ���б���
		// Ȼ���ڱ������������ʱ�򣬻�����������ڵ�ǰ�����Ķ��󣬱�����õ������ڵ�ǰ������Lua�ű�
		// ������ʱֻ֧�ֵ��������г�����ʱ����ж��ԭ����
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
		// �ȵȴ���ǰ���л��ƽ�������ж�س����ͷ���Դ
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