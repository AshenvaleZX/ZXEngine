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
		};

		class CollisionBox : public CollisionPrimitive
		{
		public:
			// Box��3�����ϵĳ��Ȱ뾶
			Vector3 mHalfSize;

			// ��ȡBoxͶӰ��ĳ�����ϵĳ��ȵ�һ��
			float GetHalfProjectedLength(const Vector3& axis) const;
		};

		// ��ײƽ��(ͨ����ʾ������һ�������ƶ���ǽ����棬�����Ǹ��壬����û�м̳�CollisionPrimitive)
		class CollisionPlane
		{
		public:
			// ��ײƽ�淨��
			Vector3 mNormal;
			// ��ײƽ�浽ԭ��ľ���
			float mDistance;
		};;
	}
}