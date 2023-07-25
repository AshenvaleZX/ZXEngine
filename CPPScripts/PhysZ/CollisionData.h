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
			// 碰撞数组
			Contact* mContactArray;
			// 当前碰撞(下一个碰撞数据要填入的位置)
			Contact* mCurContact;
			// 碰撞数组剩余空间
			uint32_t mContactsLeft;
			// 碰撞数组当前大小
			uint32_t mContactsCount;
			// 恢复系数
			float mRestitution;
			// 摩擦系数
			float mFriction;

			bool IsFull() const;
			void AddContacts(uint32_t count);
		};
	}
}