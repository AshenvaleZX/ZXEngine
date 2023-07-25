#pragma once
#include "../pubh.h"

namespace ZXEngine
{
	namespace PhysZ
	{
		class Contact;
		class CollisionData
		{
		public:
			// ��ײ����
			Contact* mContactArray;
			// ��ǰ��ײ(��һ����ײ����Ҫ�����λ��)
			Contact* mCurContact;
			// ��ײ����ʣ��ռ�
			uint32_t mContactsLeft;
			// ��ײ���鵱ǰ��С
			uint32_t mContactsCount;
			// �ָ�ϵ��
			float mRestitution;
			// Ħ��ϵ��
			float mFriction;

			bool IsFull() const;
			void AddContacts(uint32_t count);
		};
	}
}