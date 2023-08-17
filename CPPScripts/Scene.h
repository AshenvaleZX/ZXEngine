#pragma once
#include "pubh.h"

namespace ZXEngine
{
	class Camera;
	class CubeMap;
	class GameObject;
	struct SceneStruct;

	namespace PhysZ
	{
		class PScene;
	}

	class Scene
	{
	public:
		uint32_t rtPipelineID = UINT32_MAX;
		CubeMap* skyBox = nullptr;
		vector<GameObject*> gameObjects;
		RenderPipelineType renderPipelineType = RenderPipelineType::Rasterization;

		Scene(SceneStruct* sceneStruct);
		~Scene();

		void Render();
		void UpdatePhysics();

	private:
		PhysZ::PScene* mPhyScene;
		vector<Camera*> mCameras;
	};
}