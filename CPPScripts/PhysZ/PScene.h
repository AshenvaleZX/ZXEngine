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
			// ��ǰ�����е����и���
			vector<pair<GameObject*, RigidBody*>> mAllRigidBodyGO;
			// Bounding Volume Hierarchy (BVH)���ĸ��ڵ�
			BVHNode* mBVHRoot = nullptr;

			// ��ײ����
			CollisionData* mCollisionData;
			// Ǳ����ײ����
			PotentialContact* mPotentialContacts;
			// Ǳ����ײ����ĳ���
			uint32_t mMaxPotentialContacts = 1000;

			// ��ײ������
			ContactResolver* mContactResolver;

			// �򳡾�������µ���ײ��
			void AddBoundingVolume(const BoundingSphere& boundingVolume, RigidBody* rigidBody);
		};
	}
}