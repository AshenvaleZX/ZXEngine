#pragma once
#include "../pubh.h"

namespace ZXEngine
{
	namespace PhysZ
	{
		class RigidBody;
		class PScene
		{
		public:
			PScene() {};
			~PScene() {};

			void BeginFrame();
			void Update(float deltaTime);

		private:
			// ��ǰ�����е����и���
			list<RigidBody*> mAllRigidBodies;
		};
	}
}