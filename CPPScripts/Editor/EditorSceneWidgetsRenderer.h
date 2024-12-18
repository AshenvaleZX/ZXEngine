#pragma once
#include "../pubh.h"

namespace ZXEngine
{
	class Shader;
	class Material;
	class StaticMesh;
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
		uint32_t mDrawWorldTransCommandID;
		uint32_t mDrawSilhouetteCommandID;
		RenderStateSetting* mRenderStateSetting;

		GameObject* mWorldTransWidget;
		GameObject* mWorldTransWidgetCamera;

		StaticMesh* mScreenQuad;
		Shader* mSilhouetteShader;
		Shader* mSilhouetteOutlineShader;
		
		Material* mSilhouetteOutlineMaterial;
		vector<Material*> mSilhouetteMaterials;

		EditorSceneWidgetsRenderer();
		~EditorSceneWidgetsRenderer();

		void InitWorldTransWidgetCamera();
		void RenderWidget(GameObject* obj);
		void DrawObjectSilhouette(GameObject* obj, size_t& idx);
		void DrawSilhouetteOutline();
		void DrawWorldTransWidget();
	};
}