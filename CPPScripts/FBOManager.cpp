#include "FBOManager.h"
#include "RenderAPI.h"
#include "RenderEngine.h"
#include "GlobalData.h"

namespace ZXEngine
{
	FBOManager* FBOManager::mInstance = nullptr;

	void FBOManager::Create()
	{
		mInstance = new FBOManager();
		mInstance->CreateFBO("Main", FrameBufferType::Normal);
	}

	FBOManager* FBOManager::GetInstance()
	{
		return mInstance;
	}

	void FBOManager::SwitchFBO(string name)
	{
		// 直接写入到默认的屏幕Buffer
		if (name == "Screen")
		{
			RenderAPI::GetInstance()->SwitchFrameBuffer(0);
			return;
		}

		auto FBO = GetFBO(name);
		if (FBO == nullptr)
			Debug::LogError("Try to switch to an nonexistent FBO");
		else
			RenderAPI::GetInstance()->SwitchFrameBuffer(FBO->ID);
	}

	void FBOManager::CreateFBO(string name, FrameBufferType type, unsigned int width, unsigned int height)
	{
		if (allFBO.count(name) > 0)
		{
			Debug::LogError("Try to add an existing key-value");
			return;
		}
		allFBO.insert(pair<string, FrameBufferObject*>(name, RenderAPI::GetInstance()->CreateFrameBufferObject(type, width, height)));
	}

	FrameBufferObject* FBOManager::GetFBO(string name)
	{
		map<string, FrameBufferObject*>::iterator iter = allFBO.find(name);
		if (iter != allFBO.end())
			return iter->second;
		else
			return nullptr;
	}
}