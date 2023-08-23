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
			// ���鳤��
			uint32_t mMaxContactCount;
			// ��ǰ��ײ(��һ����ײ����Ҫ�����λ��)
			Contact* mCurContact;
			// ��ײ����ʣ��ռ�
			uint32_t mContactsLeft;
			// ��ײ���鵱ǰ��С
			uint32_t mCurContactCount;

			CollisionData(uint32_t maxContacts);
			~CollisionData();

			void Reset();
			bool IsFull() const;
			void AddContacts(uint32_t count);
		};
	}
}