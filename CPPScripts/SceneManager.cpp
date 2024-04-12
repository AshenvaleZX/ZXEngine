#include "SceneManager.h"
#include "Resources.h"
#include "ProjectSetting.h"
#include "RenderAPI.h"
#include "LuaManager.h"
#include "RenderPassManager.h"

#ifdef ZX_EDITOR
#include "Editor/EditorDialogBoxManager.h"
#endif

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

	Scene* SceneManager::GetScene(const string& name)
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

	void SceneManager::LoadScene(const string& path, bool switchNow)
	{
		auto sceneStruct = Resources::LoadScene(path);

		if (sceneStruct->renderPipelineType == RenderPipelineType::RayTracing)
		{
#ifdef ZX_API_OPENGL
#ifdef ZX_EDITOR
			EditorDialogBoxManager::GetInstance()->PopMessage("Notice", "OpenGL do not support ray tracing.");
#endif
			delete sceneStruct;
			return;
#else
			if (!ProjectSetting::isSupportRayTracing)
			{
#ifdef ZX_EDITOR
				EditorDialogBoxManager::GetInstance()->PopMessage("Notice", "Current GPU do not support ray tracing.");
#endif
				delete sceneStruct;
				return;
			}
#endif
		}

		// Ŀǰ�ೡ��ͬʱ���������⣬��Ҫ������GameLogic��Light������ϣ����¼��һ��ȫ���б���
		// Ȼ���ڱ������������ʱ�򣬻�����������ڵ�ǰ�����Ķ��󣬱�����õ������ڵ�ǰ������Lua�ű�
		// ������ʱֻ֧�ֵ��������г�����ʱ����ж��ԭ����
		DeleteAllScene();
		string name = Resources::GetAssetName(path);

		SceneInfo* info = new SceneInfo();
		info->path = path;
		info->scene = new Scene(sceneStruct);
		scenes.insert(pair<string, SceneInfo*>(name, info));

		if (switchNow)
			SwitchScene(name);

		delete sceneStruct;
	}

	void SceneManager::SwitchScene(const string& name)
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
			if (curScene->scene->renderPipelineType == RenderPipelineType::RayTracing)
				RenderAPI::GetInstance()->SwitchRayTracingPipeline(curScene->scene->rtPipelineID);
		}
	}

	void SceneManager::DeleteScene(const string& name)
	{
		map<string, SceneInfo*>::iterator iter = scenes.find(name);
		if (iter != scenes.end())
		{
			delete iter->second->scene;
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
			delete iter.second->scene;
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

	SceneInfo* SceneManager::GetSceneInfo(const string& name)
	{
		map<string, SceneInfo*>::iterator iter = scenes.find(name);
		if (iter != scenes.end())
			return iter->second;
		else
			return nullptr;
	}
}