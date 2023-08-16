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
			// ��ǰ�����е����и���
			list<RigidBody*> mAllRigidBodies;
			// Bounding Volume Hierarchy (BVH)���ĸ��ڵ�
			BVHNode<BoundingSphere>* mBVHRoot = nullptr;

			// Ҫ�������ײ����
			Contact* mContacts;
			// �ܴ���������ײ��(Ҳ����ײ����ĳ���)
			uint32_t mMaxContacts;

			// ��ײ������
			ContactResolver* mContactResolver;

			// �򳡾�������µ���ײ��
			void AddBoundingVolume(const BoundingSphere& boundingVolume, RigidBody* rigidBody);
		};
	}
}