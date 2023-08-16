#include "PScene.h"
#include "BVHNode.h"
#include "Contact.h"
#include "RigidBody.h"
#include "ContactResolver.h"
#include "BoundingVolume/BoundingSphere.h"
#include "../GameObject.h"

namespace ZXEngine
{
	namespace PhysZ
	{
		PScene::PScene(uint32_t maxContacts, uint32_t iterations) 
		{
			mMaxContacts = maxContacts;
			mContacts = new Contact[mMaxContacts];
			mContactResolver = new ContactResolver(iterations);
		};

		PScene::~PScene() 
		{
			delete[] mContacts;
			delete mContactResolver;
		};

		void PScene::BeginFrame()
		{
			for (auto rigidBody : mAllRigidBodies)
			{
				// 清除刚体在上一帧累计的力和力矩
				rigidBody->ClearAccumulators();
				// 更新刚体在这一帧的相关数据
				rigidBody->CalculateDerivedData();
			}
		}

		void PScene::Update(float deltaTime)
		{
			for (auto rigidBody : mAllRigidBodies)
			{
				// 更新刚体的位置和旋转
				rigidBody->Integrate(deltaTime);
			}
		}

		void PScene::AddGameObject(GameObject* gameObject)
		{
			auto rigidBodyComp = gameObject->GetComponent<ZRigidBody>();
			auto rigidBody = rigidBodyComp ? rigidBodyComp->mRigidBody : nullptr;

			if (rigidBody)
				mAllRigidBodies.push_back(rigidBody);

			if (gameObject->mColliderType == PhysZ::ColliderType::Box)
			{
				auto boxCollider = gameObject->GetComponent<BoxCollider>();
				if (boxCollider)
				{
					float radius = boxCollider->mCollider->mHalfSize.GetMagnitude();
					Vector3 pos = gameObject->GetComponent<Transform>()->GetPosition();
					BoundingSphere bv(pos, radius);
					AddBoundingVolume(bv, rigidBody);
				}
			}
			else if (gameObject->mColliderType == PhysZ::ColliderType::Plane)
			{
				auto planeCollider = gameObject->GetComponent<PlaneCollider>();
				if (planeCollider)
				{
					auto transform = gameObject->GetComponent<Transform>();
					auto scale = transform->GetLocalScale();
					// 临时按照平面长宽为10计算
					float radius = sqrtf(scale.x * scale.x * 100.0f + scale.z * scale.z * 100.0f);
					Vector3 pos = transform->GetPosition();
					BoundingSphere bv(pos, radius);
					AddBoundingVolume(bv, rigidBody);
				}
			}
			else if (gameObject->mColliderType == PhysZ::ColliderType::Sphere)
			{
				auto sphereCollider = gameObject->GetComponent<SphereCollider>();
				if (sphereCollider)
				{
					float radius = sphereCollider->mCollider->mRadius;
					Vector3 pos = gameObject->GetComponent<Transform>()->GetPosition();
					BoundingSphere bv(pos, radius);
					AddBoundingVolume(bv, rigidBody);
				}
			}

			for (auto child : gameObject->children)
			{
				AddGameObject(child);
			}
		}

		void PScene::AddBoundingVolume(const BoundingSphere& boundingVolume, RigidBody* rigidBody)
		{
			if (mBVHRoot)
				mBVHRoot->Insert(boundingVolume, rigidBody);
			else
				mBVHRoot = new BVHNode<BoundingSphere>(nullptr, boundingVolume, rigidBody);
		}
	}
}
