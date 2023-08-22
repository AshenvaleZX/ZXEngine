#pragma once

namespace ZXEngine
{
	namespace PhysZ
	{
		// ���������˶������ڴ�ֵ�ͻ��������״̬
		// ���߱����������Ż����ܵģ����ֵԽ��Խ���׽�������״̬������̫���˻ᵼ��ģ��ʧ��
		// ���������ֵӦ���ڲ����ģ��ʧ���ǰ���¾����ܵĴ�
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