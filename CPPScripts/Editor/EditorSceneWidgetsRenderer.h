#pragma once
#include "../pubh.h"

namespace ZXEngine
{
	class GameObject;
	class RenderStateSetting;
	class EditorSceneWidgetsRenderer
	{
	public:
		static void Create();
		static EditorSceneWidgetsRenderer* GetInstance();

	private:
		static EditorSceneWidgetsRenderer* mInstance;

	public:
		void Render();

	private:
		uint32_t mDrawCommandID;
		RenderStateSetting* mRenderStateSetting;

		EditorSceneWidgetsRenderer();
		~EditorSceneWidgetsRenderer();

		void RenderObject(GameObject* obj);
	};
}