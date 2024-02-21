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

		void Update();
		void Render();

	private:
		vector<Camera*> mCameras;

		PhysZ::PScene* mPhyScene;
		long long mCurPhyFrame = 0;

		void UpdatePhysics();
	};
}