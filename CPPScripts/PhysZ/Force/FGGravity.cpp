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
			// ����������޴��ʾ������
			if (rigidBody->IsInfiniteMass()) 
				return;

			// �������
			rigidBody->AddForce(mGravity * rigidBody->GetMass());
		}
	}
}