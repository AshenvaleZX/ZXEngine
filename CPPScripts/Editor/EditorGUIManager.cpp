#include "EditorGUIManager.h"
#ifdef ZX_API_OPENGL
#include "EditorGUIManagerOpenGL.h"
#endif
#ifdef ZX_API_VULKAN
#include "EditorGUIManagerVulkan.h"
#endif

namespace ZXEngine
{
	EditorGUIManager* EditorGUIManager::mInstance = nullptr;

	void EditorGUIManager::Create()
	{
#ifdef ZX_API_OPENGL
		mInstance = new EditorGUIManagerOpenGL();
#endif
#ifdef ZX_API_VULKAN
		mInstance = new EditorGUIManagerVulkan();
#endif
	}

	EditorGUIManager* EditorGUIManager::GetInstance()
	{
		return mInstance;
	}
}