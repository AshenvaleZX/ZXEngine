#include "FGGravity.h"

namespace ZXEngine
{
	namespace PhysZ
	{
		void FGGravity::UpdateForce(RigidBody* rigidBody, float duration)
		{
			// ����������޴��ʾ������
			if (rigidBody->IsInfiniteMass()) 
				return;

			// �������
			rigidBody->AddForce(gravity * rigidBody->GetMass());
		}
	}
}