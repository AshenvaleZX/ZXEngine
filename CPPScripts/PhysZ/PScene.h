#pragma once
#include "../pubh.h"
#include "BVHNode.h"

namespace ZXEngine
{
	class Cloth;
	class GameObject;
	namespace PhysZ
	{
		class BoundingSphere;
		class Contact;
		class RigidBody;
		class CollisionData;
		class ContactResolver;
		class PScene
		{
		public:
			PScene(uint32_t maxContacts, uint32_t iterations = 0);
			~PScene();

			void BeginFrame();
			void Update(float deltaTime);
			void EndFrame();

			void AddGameObject(GameObject* gameObject);

		private:
			// 当前场景中的所有布料
			vector<Cloth*> mAllCloths;
			// 当前场景中的所有刚体
			vector<pair<GameObject*, RigidBody*>> mAllRigidBodyGO;
			// Bounding Volume Hierarchy (BVH)树的根节点
			BVHNode* mBVHRoot = nullptr;

			// 碰撞数据
			CollisionData* mCollisionData;
			// 潜在碰撞数组
			PotentialContact* mPotentialContacts;
			// 潜在碰撞数组的长度
			uint32_t mMaxPotentialContacts = 1000;

			// 碰撞处理器
			ContactResolver* mContactResolver;

			// 向场景中添加新的碰撞体
			void AddBoundingVolume(const BoundingSphere& boundingVolume, RigidBody* rigidBody);
		};
	}
}