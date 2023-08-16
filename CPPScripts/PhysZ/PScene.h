#pragma once
#include "../pubh.h"
#include "BVHNode.h"

namespace ZXEngine
{
	class GameObject;
	namespace PhysZ
	{
		class BoundingSphere;
		class Contact;
		class RigidBody;
		class ContactResolver;
		class PScene
		{
		public:
			PScene(uint32_t maxContacts, uint32_t iterations = 0);
			~PScene();

			void BeginFrame();
			void Update(float deltaTime);

			void AddGameObject(GameObject* gameObject);

		private:
			// 当前场景中的所有刚体
			list<RigidBody*> mAllRigidBodies;
			// Bounding Volume Hierarchy (BVH)树的根节点
			BVHNode<BoundingSphere>* mBVHRoot = nullptr;

			// 要处理的碰撞数组
			Contact* mContacts;
			// 能处理的最大碰撞数(也是碰撞数组的长度)
			uint32_t mMaxContacts;

			// 碰撞处理器
			ContactResolver* mContactResolver;

			// 向场景中添加新的碰撞体
			void AddBoundingVolume(const BoundingSphere& boundingVolume, RigidBody* rigidBody);
		};
	}
}