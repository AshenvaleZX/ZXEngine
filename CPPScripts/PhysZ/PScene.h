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
			// 当前场景中的所有刚体
			list<RigidBody*> mAllRigidBodies;
		};
	}
}