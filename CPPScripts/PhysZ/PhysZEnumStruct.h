#pragma once

namespace ZXEngine
{
	namespace PhysZ
	{
		// ���������˶������ڴ�ֵ�ͻ��������״̬
		// ���߱����������Ż����ܵģ����ֵԽ��Խ���׽�������״̬������̫���˻ᵼ��ģ��ʧ��
		// ���������ֵӦ���ڲ����ģ��ʧ���ǰ���¾����ܵĴ�
		static float SleepMotionEpsilon = 0.1f;

		class RigidBody;
		struct PotentialContact
		{
			RigidBody* bodies[2];
		};
	}
}