#pragma once
#include "ForceGenerator.h"

namespace ZXEngine
{
	namespace PhysZ
	{
		class FGSpring : public ForceGenerator
		{
		public:
			FGSpring(const Vector3& connectionPoint, const Vector3& otherConnectionPoint, RigidBody* other, float springConstant, float restLength);

			virtual void UpdateForce(RigidBody* rigidBody, float duration);

		private:
			// 弹簧在当前对象上的连接点(对象的局部空间)
			Vector3 mConnectionPoint;
			// 弹簧在另一个对象上的连接点(另一个对象的局部空间)
			Vector3 mOtherConnectionPoint;
			// 弹簧另一端的刚体
			RigidBody* mOther;
			// 弹簧系数
			float mSpringConstant;
			// 弹簧不受外力时的长度
			float mRestLength;
		};
	}
}