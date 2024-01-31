#include "PScene.h"
#include "BVHNode.h"
#include "Contact.h"
#include "RigidBody.h"
#include "CollisionData.h"
#include "CollisionDetector.h"
#include "ContactResolver.h"
#include "BoundingVolume/BoundingSphere.h"
#include "../GameObject.h"
#include "../DynamicMesh.h"

namespace ZXEngine
{
	namespace PhysZ
	{
		PScene::PScene(uint32_t maxContacts, uint32_t iterations) 
		{
			mCollisionData = new CollisionData(1000);
			mContactResolver = new ContactResolver(iterations);
			mPotentialContacts = new PotentialContact[mMaxPotentialContacts];
		};

		PScene::~PScene() 
		{
			delete mCollisionData;
			delete mContactResolver;
			delete[] mPotentialContacts;

			if (mBVHRoot)
				delete mBVHRoot;
		};

		void PScene::BeginFrame()
		{
			if (mBVHRoot == nullptr)
				return;

			// ������ײ����
			mCollisionData->Reset();

			for (auto& iter : mAllRigidBodyGO)
			{
				// �����������һ֡�ۼƵ���������
				iter.second->ClearAccumulators();
				// ���¸�������һ֡���������
				iter.second->CalculateDerivedData();
			}

			for (auto cloth : mAllCloths)
			{
				for (auto& iter : cloth->mParticles)
				{
					iter.first->ClearAccumulators();
					iter.first->CalculateDerivedData();
				}
			}
		}

		void PScene::Update(float deltaTime)
		{
			if (mBVHRoot == nullptr)
				return;

			for (auto& iter : mAllRigidBodyGO)
			{
				// ���¸����λ�ú���ת
				iter.second->Integrate(deltaTime);
			}

			for (auto cloth : mAllCloths)
			{
				for (auto& iter : cloth->mParticles)
				{
					iter.first->Integrate(deltaTime);
				}
			}

			// ����Ǳ����ײ
			uint32_t potentialContactCount = mBVHRoot->GetPotentialContacts(mPotentialContacts, mMaxPotentialContacts);
			
			// ��Ǳ����ײ�м����ײ
			uint32_t i = 0;
			while (i < potentialContactCount)
			{
				uint32_t collisionCount = CollisionDetector::Detect(
					mPotentialContacts[i].mRigidBodies[0]->mCollisionVolume, 
					mPotentialContacts[i].mRigidBodies[1]->mCollisionVolume, 
					mCollisionData
				);
				i++;
			}

			// ������ײ
			mContactResolver->ResolveContacts(mCollisionData->mContactArray, mCollisionData->mCurContactCount, deltaTime);
		}

		void PScene::EndFrame()
		{
			if (mBVHRoot == nullptr)
				return;

			for (auto& iter : mAllRigidBodyGO)
			{
				auto transform = iter.first->GetComponent<Transform>();
				transform->SetPosition(iter.second->GetPosition());
				transform->SetRotation(iter.second->GetRotation());

				// ����BV��λ��
				if (iter.second->mBVHNode)
				{
					iter.second->mBVHNode->mBoundingVolume.mCenter = iter.second->GetPosition();
					iter.second->mBVHNode->UpdateBoundingVolume();
				}
			}

			for (auto cloth : mAllCloths)
			{
				Vector3 wPos = cloth->gameObject->GetComponent<Transform>()->GetPosition();

				for (size_t i = 0; i < cloth->mParticles.size(); i++)
				{
					Vector3 pPos = cloth->mParticles[i].first->GetPosition();
					Vector3 lPos = pPos - wPos;

					cloth->mDynamicMesh->mVertices[i].Position = lPos;

					// ����BV��λ��
					if (cloth->mParticles[i].first->mBVHNode)
					{
						cloth->mParticles[i].first->mBVHNode->mBoundingVolume.mCenter = pPos;
						cloth->mParticles[i].first->mBVHNode->UpdateBoundingVolume();
					}
				}

				// ����(Z)
				size_t row = 11;
				// ����(X)
				size_t col = 11;
				// �Ӻ���ǰ
				for (size_t i = 1; i < (row - 1); i++)
				{
					// ��������
					for (size_t j = 1; j < (col - 1); j++)
					{
						// ��
						size_t pLeft = i * col + j - 1;
						// ��
						size_t pRight = i * col + j + 1;
						// ��
						size_t pBack = (i - 1) * col + j;
						// ǰ
						size_t pFront = (i + 1) * col + j;

						if (i == 0)
							pBack = i * col + j;
						else if (i == (row - 1))
							pFront = i * col + j;

						if (j == 0)
							pLeft = i * col + j;
						else if (j == (col - 1))
							pRight = i * col + j;

						Vector3 normal = Math::Cross(
							cloth->mDynamicMesh->mVertices[pLeft].Position - cloth->mDynamicMesh->mVertices[pRight].Position,
							cloth->mDynamicMesh->mVertices[pBack].Position - cloth->mDynamicMesh->mVertices[pFront].Position
						);
						normal.Normalize();

						cloth->mDynamicMesh->mVertices[i * col + j].Normal = normal;
					}
				}

				cloth->mDynamicMesh->UpdateData();
			}
		}

		void PScene::AddGameObject(GameObject* gameObject)
		{
			auto rigidBodyComp = gameObject->GetComponent<ZRigidBody>();
			auto rigidBody = rigidBodyComp ? rigidBodyComp->mRigidBody : nullptr;

			if (rigidBody)
				mAllRigidBodyGO.push_back(pair(gameObject, rigidBody));

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
					// ��ʱ����ƽ�泤��Ϊ10����
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
			else if (gameObject->mColliderType == PhysZ::ColliderType::Cloth)
			{
				auto cloth = gameObject->GetComponent<Cloth>();

				for (auto& iter : cloth->mParticles)
				{
					float radius = iter.second->mHalfSize.GetMagnitude();
					Vector3 pos = iter.first->GetPosition();
					BoundingSphere bv(pos, radius);
					AddBoundingVolume(bv, iter.first);
				}

				mAllCloths.push_back(cloth);
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
				mBVHRoot = new BVHNode(nullptr, boundingVolume, rigidBody);
		}
	}
}
