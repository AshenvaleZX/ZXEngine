#include "FGGravity.h"
#include "../RigidBody.h"

namespace ZXEngine
{
	namespace PhysZ
	{
		FGGravity::FGGravity(const Vector3& gravity) : mGravity(gravity)
		{ 
			mType = ForceGeneratorType::Gravity;
		}

		void FGGravity::UpdateForce(RigidBody* rigidBody, float duration)
		{
			// 如果质量无限大表示不受力
			if (rigidBody->IsInfiniteMass()) 
				return;

			// 添加重力
			rigidBody->AddForce(mGravity * rigidBody->GetMass());
		}
	}
}