#pragma once
#include "pubh.h"

namespace ZXEngine
{
	class Camera;
	class CubeMap;
	class GameObject;
	struct SceneStruct;
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

	private:
		vector<Camera*> cameras;
	};
}