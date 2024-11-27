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

		// ��������Ƶ��(ÿ��)
		float mRate = 10.0f;
		// ���ӷ�����״
		Shape mShape = Shape::Cone;
		// ���ӷ���Ƕ�
		float mAngle = 30.0f;
		// ���ӷ���뾶
		float mRadius = 1.0f;
	};

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

		// �����ص�GO��ǰλ��
		Vector3 mCurPos;
		// �����ص�GO��һ֡λ��
		Vector3 mLastPos;
		// �����ص�GO��ǰ�ƶ�����
		Vector3 mMoveDir;
	};
}