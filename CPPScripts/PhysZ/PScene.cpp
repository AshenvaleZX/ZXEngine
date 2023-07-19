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
	}
}
