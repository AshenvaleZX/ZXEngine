#pragma once
#include "../pubh.h"
#include "PhysZEnumStruct.h"

namespace ZXEngine
{
	namespace PhysZ
	{
		class RigidBody;
		class CollisionPrimitive
		{
		public:
			// ��ײ������Ӧ�ĸ���
			RigidBody* mRigidBody = nullptr;
			// ��ײ������ڸ����ƫ��(��������ȫ������ʹ�õ�λ���󼴿�)
			Matrix4 mOffset;
			// ��ײ�����������ı任����
			Matrix4 mTransform;
			// Ħ��ϵ��
			float mFriction = 0.4f;
			// �ص�ϵ��
			float mBounciness = 0.0f;
			// Ħ��ϵ����Ϸ�ʽ
			CombineType mFrictionCombine = CombineType::Average;
			// �ص�ϵ����Ϸ�ʽ
			CombineType mBounceCombine = CombineType::Average;

			virtual ~CollisionPrimitive();

			virtual void SynchronizeTransform(const Matrix4& transform);

			// ��ȡ��ײ������
			virtual ColliderType GetType() const = 0;
			// ���ɶ�Ӧ��״�ľ��ʹ�������
			virtual Matrix3 GetInertiaTensor(float mass) const = 0;
		};

		class CollisionBox : public CollisionPrimitive
		{
		public:
			// Box��3�����ϵĳ��Ȱ뾶
			Vector3 mHalfSize;

			virtual ColliderType GetType() const;
			virtual Matrix3 GetInertiaTensor(float mass) const;

			// ��ȡBoxͶӰ��ĳ�����ϵĳ��ȵ�һ��
			float GetHalfProjectedLength(const Vector3& axis) const;
		};

		// ��ײƽ��(ͨ����ʾ������һ�������ƶ���ǽ����棬�����Ǹ���)
		class CollisionPlane : public CollisionPrimitive
		{
		public:
			// ��ײƽ�淨��(World)
			Vector3 mNormal;
			// ��ײƽ�淨��(Local)
			Vector3 mLocalNormal;
			// ��ײƽ�浽ԭ��ľ���
			float mDistance;

			virtual void SynchronizeTransform(const Matrix4& transform);

			virtual ColliderType GetType() const;
			virtual Matrix3 GetInertiaTensor(float mass) const;
		};

		class CollisionSphere : public CollisionPrimitive
		{
		public:
			// ��ײ��뾶
			float mRadius;

			virtual ColliderType GetType() const;
			virtual Matrix3 GetInertiaTensor(float mass) const;
		};

		struct CollisionVolume
		{
			// ��ײ������
			ColliderType mType;
			// ��ײ��ָ��
			union
			{
				CollisionBox* mBox;
				CollisionPlane* mPlane;
				CollisionSphere* mSphere;
			};
		};
	}
}