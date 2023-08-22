#pragma once

namespace ZXEngine
{
	namespace PhysZ
	{
		// 如果刚体的运动量低于此值就会进入休眠状态
		// 休眠本来是用于优化性能的，这个值越大越容易进入休眠状态，但是太大了会导致模拟失真
		// 所以这个数值应该在不造成模拟失真的前提下尽可能的大
		static float SleepMotionEpsilon = 0.01f;

		enum class ColliderType
		{
			None,
			Box,
			Plane,
			Sphere,
		};

		enum class CombineType
		{
			Average,
			Minimum,
			Maximum,
			Multiply,
		};

		class RigidBody;
		struct PotentialContact
		{
			RigidBody* mRigidBodies[2];
		};
	}
}