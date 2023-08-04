#pragma once
#include "../pubh.h"

namespace ZXEngine
{
	namespace PhysZ
	{
		class Contact;
		class ContactResolver
		{
		public:
			// ��ǰ�����ٶ��Ѿ��ĵ������Ĵ���
			uint32_t mCurVelocityIterations;
			// ��ǰ����λ���Ѿ��ĵ������Ĵ���
			uint32_t mCurPositionIterations;

			ContactResolver(uint32_t maxIterations, float velocityEpsilon = 0.01f, float positionEpsilon = 0.01f);
			ContactResolver(uint32_t maxVelocityIterations, uint32_t maxPositionIterations, float velocityEpsilon = 0.01f, float positionEpsilon = 0.01f);

			void SetMaxIterations(uint32_t maxIterations);
			void SetVelocityEpsilon(float velocityEpsilon);
			void SetPositionEpsilon(float positionEpsilon);

		private:
			float mVelocityEpsilon;
			// ����������ײʱ���ཻ������С�����ֵ������Ϊ����ײ
			// ���ֵ���̫С���ᵼ�¶�����̫���˻ᵼ�����ۿɼ��Ķ����ཻ
			float mPositionEpsilon;
			// �����ٶ�ʱ������������
			uint32_t mMaxVelocityIterations;
			// ����λ��ʱ������������
			uint32_t mMaxPositionIterations;
		};
	}
}