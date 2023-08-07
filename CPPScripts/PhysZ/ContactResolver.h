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
			// 当前处理速度已经的迭代过的次数
			uint32_t mCurVelocityIterations;
			// 当前处理位置已经的迭代过的次数
			uint32_t mCurPositionIterations;

			ContactResolver(uint32_t maxIterations, float velocityEpsilon = 0.01f, float positionEpsilon = 0.01f);
			ContactResolver(uint32_t maxVelocityIterations, uint32_t maxPositionIterations, float velocityEpsilon = 0.01f, float positionEpsilon = 0.01f);

			bool IsValid();

			void SetMaxIterations(uint32_t maxIterations);
			void SetVelocityEpsilon(float velocityEpsilon);
			void SetPositionEpsilon(float positionEpsilon);

			void ResolveContacts(Contact* contacts, uint32_t numContacts, float duration);

		private:
			float mVelocityEpsilon;
			// 两个对象碰撞时的相交深度如果小于这个值，则认为不碰撞
			// 这个值如果太小，会导致抖动，太大了会导致肉眼可见的对象相交
			float mPositionEpsilon;
			// 处理速度时的最大迭代次数
			uint32_t mMaxVelocityIterations;
			// 处理位置时的最大迭代次数
			uint32_t mMaxPositionIterations;

			// 处理碰撞前先更新碰撞数据
			void PrepareContacts(Contact* contacts, uint32_t numContacts, float duration);
			// 处理碰撞相交
			void AdjustPositions(Contact* contacts, uint32_t numContacts, float duration);
			// 处理碰撞速度变化
			void AdjustVelocities(Contact* contacts, uint32_t numContacts, float duration);
		};
	}
}