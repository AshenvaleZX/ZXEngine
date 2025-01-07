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

		// ��������Ƶ��(ÿ��)
		float mRate = 10.0f;
		// ���ӷ�����״
		Shape mShape = Shape::Cone;
		// ���ӷ���Ƕ�(�Ƕ���)
		float mAngle = 30.0f;
		// ���ӷ���뾶
		float mRadius = 1.0f;
		// ���ӷ�����ɫ
		Vector4 mColor = Vector4(1.0f);
		// �Ƿ������ɫ
		bool mRandomColor = false;
		// ���ӷ����ٶ�
		float mSpeed = 1.0f;
		// ���ӷ��䷽����ת
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
		// �����������
		uint32_t mMaxParticleNum = 0;
		// ������������
		float mLifeTime = 0;
		// �����ƶ�������ٶ�
		Vector3 mVelocity;
		// ��������λ��ƫ����
		Vector3 mOffset;
		// ���ӷ���״̬
		ParticleEmissionState mEmissionState;

		// �����ص�GO��ǰλ��(World Space)
		Vector3 mCurPos;
		// �����ص�GO��ǰ�ƶ�����(World Space)
		Vector3 mMoveDir;
		// �����ص�GO��ǰ��ת(�����World Space)
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