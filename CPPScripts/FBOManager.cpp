#include "FBOManager.h"
#include "RenderAPI.h"
#include "RenderEngine.h"
#include "GlobalData.h"
#include "ProjectSetting.h"

namespace ZXEngine
{
	FBOManager* FBOManager::mInstance = nullptr;

	void FBOManager::Create()
	{
		mInstance = new FBOManager();

		ClearInfo clearInfo = {};
		clearInfo.clearFlags = ZX_CLEAR_FRAME_BUFFER_COLOR_BIT | ZX_CLEAR_FRAME_BUFFER_DEPTH_BIT;
		mInstance->CreateFBO("Main", FrameBufferType::Normal, clearInfo);
	}

	FBOManager* FBOManager::GetInstance()
	{
		return mInstance;
	}

	void FBOManager::SwitchFBO(const string& name)
	{
		// ֱ��д�뵽Ĭ�ϵ���ĻBuffer
		if (name == ScreenBuffer)
		{
#ifdef ZX_EDITOR
			RenderAPI::GetInstance()->SetViewPort(GlobalData::srcWidth, GlobalData::srcHeight, ProjectSetting::hierarchyWidth, ProjectSetting::projectHeight);
#endif
			RenderAPI::GetInstance()->SwitchFrameBuffer(UINT32_MAX);
			return;
		}

		auto FBO = GetFBO(name);
		if (FBO == nullptr)
			Debug::LogError("Try to switch to an nonexistent FBO");
		else
			RenderAPI::GetInstance()->SwitchFrameBuffer(FBO->ID);
	}

	void FBOManager::CreateFBO(const string& name, FrameBufferType type, unsigned int width, unsigned int height)
	{
		ClearInfo clearInfo = {};
		CreateFBO(name, type, clearInfo, width, height);
	}

	void FBOManager::CreateFBO(const string& name, FrameBufferType type, const ClearInfo& clearInfo, unsigned int width, unsigned int height)
	{
		if (allFBO.count(name) > 0)
		{
			Debug::LogError("Try to add an existing key-value");
			return;
		}
		allFBO.insert(pair<string, FrameBufferObject*>(name, RenderAPI::GetInstance()->CreateFrameBufferObject(type, clearInfo, width, height)));
	}

	FrameBufferObject* FBOManager::GetFBO(const string& name)
	{
		map<string, FrameBufferObject*>::iterator iter = allFBO.find(name);
		if (iter != allFBO.end())
			return iter->second;
		else
			return nullptr;
	}
}