#include "FGGravity.h"

namespace ZXEngine
{
	namespace PhysZ
	{
		void FGGravity::UpdateForce(RigidBody* rigidBody, float duration)
		{
			// 如果质量无限大表示不受力
			if (rigidBody->IsInfiniteMass()) 
				return;

			// 添加重力
			rigidBody->AddForce(gravity * rigidBody->GetMass());
		}
	}
}