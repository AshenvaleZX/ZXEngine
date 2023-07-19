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
			// �����ڵ�ǰ�����ϵ����ӵ�(����ľֲ��ռ�)
			Vector3 mConnectionPoint;
			// ��������һ�������ϵ����ӵ�(��һ������ľֲ��ռ�)
			Vector3 mOtherConnectionPoint;
			// ������һ�˵ĸ���
			RigidBody* mOther;
			// ����ϵ��
			float mSpringConstant;
			// ���ɲ�������ʱ�ĳ���
			float mRestLength;
		};
	}
}