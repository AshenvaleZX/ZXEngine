#include "Cloth.h"
#include "../../GameObject.h"
#include "../../DynamicMesh.h"

namespace ZXEngine
{
	ComponentType Cloth::GetType()
	{
		return ComponentType::Cloth;
	}

	ComponentType Cloth::GetInsType()
	{
		return ComponentType::Cloth;
	}

	void Cloth::Init()
	{
		mDynamicMesh = static_cast<DynamicMesh*>(gameObject->GetComponent<MeshRenderer>()->mMeshes[0]);

		float mass = mMass / mDynamicMesh->mVertices.size();
		Vector3 wPos = gameObject->GetComponent<Transform>()->GetPosition();

		for (size_t i = 0; i < mDynamicMesh->mVertices.size(); i++)
		{
			PhysZ::CollisionBox* sphere = new PhysZ::CollisionBox();
			sphere->mFriction = mFriction;
			sphere->mBounciness = 0.0f;
			sphere->mHalfSize = Vector3(0.1f);

			PhysZ::RigidBody* body = new PhysZ::RigidBody();
			if (mUseGravity)
			{
				auto fgGravity = new PhysZ::FGGravity(Vector3(0.0f, -9.8f, 0.0f));
				body->AddForceGenerator(fgGravity);
			}
			body->SetMass(mass);
			body->SetPosition(wPos + mDynamicMesh->mVertices[i].Position);

			sphere->mRigidBody = body;
			body->mCollisionVolume = sphere;
			body->SetInertiaTensor(sphere->GetInertiaTensor(mass));

			mParticles.push_back(pair(body, sphere));
		}

		// 行数(Z)
		uint32_t row = 11;
		// 列数(X)
		uint32_t col = 11;
		for (uint32_t i = 0; i < row; i++)
		{
			for (uint32_t j = 0; j < col; j++)
			{
				uint32_t otherIdx = 0;
				uint32_t curIdx = i * col + j;

				// 右边一个
				if (j < col - 1)
				{
					otherIdx = curIdx + 1;
					AddInternalSpring(curIdx, otherIdx, mStretchStiffness);
				}

				// 下边一个
				if (i < row - 1)
				{
					otherIdx = curIdx + col;
					AddInternalSpring(curIdx, otherIdx, mStretchStiffness);
				}

				// 右下角一个
				if (i < row - 1 && j < col - 1)
				{
					otherIdx = curIdx + col + 1;
					AddInternalSpring(curIdx, otherIdx, mStretchStiffness);
				}

				// 左下角一个
				if (i < row - 1 && j > 0)
				{
					otherIdx = curIdx + col - 1;
					AddInternalSpring(curIdx, otherIdx, mStretchStiffness);
				}

				// 右边隔一个
				if (j < col - 2)
				{
					otherIdx = curIdx + 2;
					AddInternalSpring(curIdx, otherIdx, mBendStiffness);
				}

				// 下边隔一个
				if (i < row - 2)
				{
					otherIdx = curIdx + col * 2;
					AddInternalSpring(curIdx, otherIdx, mBendStiffness);
				}
			}
		}
	}

	void Cloth::AddInternalSpring(uint32_t idx1, uint32_t idx2, float constant)
	{
		float length = (mDynamicMesh->mVertices[idx1].Position - mDynamicMesh->mVertices[idx2].Position).GetMagnitude();

		auto fgSpring1 = new PhysZ::FGSpring(
			Vector3::Zero, Vector3::Zero,
			mParticles[idx2].first,
			constant, length
		);
		mParticles[idx1].first->AddForceGenerator(fgSpring1);

		auto fgSpring2 = new PhysZ::FGSpring(
			Vector3::Zero, Vector3::Zero,
			mParticles[idx1].first,
			constant, length
		);
		mParticles[idx2].first->AddForceGenerator(fgSpring2);
	}
}