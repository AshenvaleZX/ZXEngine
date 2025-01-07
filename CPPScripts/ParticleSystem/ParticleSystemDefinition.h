#pragma once
#include "../pubh.h"
#include "../Reflection/StaticReflection.h"

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
			Box,
			Line,
		};

		// 粒子生成频率(每秒)
		float mRate = 10.0f;
		// 粒子发射形状
		Shape mShape = Shape::Cone;
		// 粒子发射角度(角度制)
		float mAngle = 30.0f;
		// 粒子发射半径
		float mRadius = 1.0f;
		// 粒子发射颜色
		Vector4 mColor = Vector4(1.0f);
		// 是否随机颜色
		bool mRandomColor = false;
		// 粒子发射速度
		float mSpeed = 1.0f;
		// 粒子发射方向旋转
		Quaternion mRotation;
	};

	ZXRef_StaticReflection
	(
		ParticleEmissionState,
		ZXRef_Fields
		(
			ZXRef_Field(&ParticleEmissionState::mRate),
			ZXRef_Field(&ParticleEmissionState::mShape),
			ZXRef_Field(&ParticleEmissionState::mAngle),
			ZXRef_Field(&ParticleEmissionState::mColor),
			ZXRef_Field(&ParticleEmissionState::mRandomColor),
			ZXRef_Field(&ParticleEmissionState::mSpeed),
			ZXRef_Field(&ParticleEmissionState::mRotation)
		)
	)

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

		// 所挂载的GO当前位置(World Space)
		Vector3 mCurPos;
		// 所挂载的GO当前移动方向(World Space)
		Vector3 mMoveDir;
		// 所挂载的GO当前旋转(相对于World Space)
		Quaternion mCurRot;
	};

	ZXRef_StaticReflection
	(
		ParticleSystemState,
		ZXRef_Fields
		(
			ZXRef_Field(&ParticleSystemState::mMaxParticleNum),
			ZXRef_Field(&ParticleSystemState::mLifeTime),
			ZXRef_Field(&ParticleSystemState::mVelocity),
			ZXRef_Field(&ParticleSystemState::mOffset),
			ZXRef_Field(&ParticleSystemState::mEmissionState),
			ZXRef_Field(&ParticleSystemState::mCurPos),
			ZXRef_Field(&ParticleSystemState::mMoveDir),
			ZXRef_Field(&ParticleSystemState::mCurRot)
		)
	)
}