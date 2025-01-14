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
		class Ray;
		class PScene;
		struct RayHitInfo;
	}

	class Scene
	{
	public:
		uint32_t rtPipelineID = UINT32_MAX;
		CubeMap* skyBox = nullptr;
		vector<GameObject*> gameObjects;
		RenderPipelineType renderPipelineType = RenderPipelineType::Forward;

		Scene(SceneStruct* sceneStruct);
		~Scene();

		void Update();
		void Render();
		GameObject* Pick(const PhysZ::Ray& ray);
		// 添加到场景根节点
		void AddGameObject(GameObject* gameObject);
		// 从场景根节点移除
		void RemoveGameObject(GameObject* gameObject);

	private:
		bool mIsAwake = false;
		vector<Camera*> mCameras;

		PhysZ::PScene* mPhyScene;
		long long mCurPhyFrame = 0;

		void UpdatePhysics();
		GameObject* Pick(const PhysZ::Ray& ray, GameObject* gameObject, PhysZ::RayHitInfo& hit);
	};
}