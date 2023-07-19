#include "PScene.h"
#include "RigidBody.h"

namespace ZXEngine
{
	namespace PhysZ
	{
		void PScene::BeginFrame()
		{
			for (auto rigidBody : mAllRigidBodies)
			{
				// �����������һ֡�ۼƵ���������
				rigidBody->ClearAccumulators();
				// ���¸�������һ֡���������
				rigidBody->CalculateDerivedData();
			}
		}

		void PScene::Update(float deltaTime)
		{
			for (auto rigidBody : mAllRigidBodies)
			{
				// ���¸����λ�ú���ת
				rigidBody->Integrate(deltaTime);
			}
		}
	}
}
