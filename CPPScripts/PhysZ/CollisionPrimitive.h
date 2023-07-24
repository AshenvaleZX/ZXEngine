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
			// 碰撞体所对应的刚体
			RigidBody* mRigidBody;
			// 碰撞体相对于刚体的偏移(若二者完全对齐则使用单位矩阵即可)
			Matrix4 mOffset;
			// 碰撞体相对于世界的变换矩阵
			Matrix4 mTransform;
		};

		class CollisionSphere : public CollisionPrimitive
		{
		public:
			// 碰撞球半径
			float mRadius;
		};;
	}
}