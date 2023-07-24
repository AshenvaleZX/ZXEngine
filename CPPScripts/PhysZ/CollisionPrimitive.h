#pragma once
#include "../pubh.h"

namespace ZXEngine
{
	namespace PhysZ
	{
		class RigidBody;
		class CollisionPrimitive
		{
		public:
			// ��ײ������Ӧ�ĸ���
			RigidBody* mRigidBody;
			// ��ײ������ڸ����ƫ��(��������ȫ������ʹ�õ�λ���󼴿�)
			Matrix4 mOffset;
			// ��ײ�����������ı任����
			Matrix4 mTransform;
		};

		class CollisionSphere : public CollisionPrimitive
		{
		public:
			// ��ײ��뾶
			float mRadius;
		};;
	}
}