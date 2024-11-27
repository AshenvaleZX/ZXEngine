#pragma once
#include "../pubh.h"

namespace ZXEngine
{
	struct Particle
	{
		Vector3 position;
		Vector3 velocity;
		Vector4 color;
		float life = 0.0f;
	};

	struct ParticleInstanceData
	{
		Matrix4 model;
		Vector4 color;
	};

	struct ParticleEmissionState
	{
		enum class Shape
		{
			Sphere,
			Hemisphere,
			Cone,
			Circle,
		};

		// 粒子生成频率(每秒)
		float mRate = 10.0f;
		// 粒子发射形状
		Shape mShape = Shape::Cone;
		// 粒子发射角度
		float mAngle = 30.0f;
		// 粒子发射半径
		float mRadius = 1.0f;
	};

	struct ParticleSystemState
	{
		// 最大粒子数量
		uint32_t mMaxParticleNum = 0;
		// 粒子生命周期
		float mLifeTime = 0;
		// 粒子移动方向和速度
		Vector3 mVelocity;
		// 粒子生成位置偏移量
		Vector3 mOffset;
		// 粒子发射状态
		ParticleEmissionState mEmissionState;

		// 所挂载的GO当前位置
		Vector3 mCurPos;
		// 所挂载的GO上一帧位置
		Vector3 mLastPos;
		// 所挂载的GO当前移动方向
		Vector3 mMoveDir;
	};
}