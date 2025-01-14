#include "ContactResolver.h"
#include "Contact.h"

namespace ZXEngine
{
	namespace PhysZ
	{
		ContactResolver::ContactResolver(uint32_t maxIterations, float velocityEpsilon, float positionEpsilon) :
			mMaxVelocityIterations(maxIterations),
			mMaxPositionIterations(maxIterations),
			mCurVelocityIterations(0),
			mCurPositionIterations(0),
			mVelocityEpsilon(velocityEpsilon), 
			mPositionEpsilon(positionEpsilon)
		{}

		ContactResolver::ContactResolver(uint32_t maxVelocityIterations, uint32_t maxPositionIterations, float velocityEpsilon, float positionEpsilon) :
			mMaxVelocityIterations(maxVelocityIterations),
			mMaxPositionIterations(maxPositionIterations),
			mCurVelocityIterations(0),
			mCurPositionIterations(0),
			mVelocityEpsilon(velocityEpsilon), 
			mPositionEpsilon(positionEpsilon)
		{}

		bool ContactResolver::IsValid()
		{
			return (mMaxVelocityIterations > 0) &&
				(mMaxPositionIterations > 0) &&
				(mVelocityEpsilon >= 0.0f) &&
				(mPositionEpsilon >= 0.0f);
		}

		void ContactResolver::SetMaxIterations(uint32_t maxIterations)
		{
			mMaxVelocityIterations = maxIterations;
			mMaxPositionIterations = maxIterations;
		}

		void ContactResolver::SetVelocityEpsilon(float velocityEpsilon)
		{
			mVelocityEpsilon = velocityEpsilon;
		}

		void ContactResolver::SetPositionEpsilon(float positionEpsilon)
		{
			mPositionEpsilon = positionEpsilon;
		}

		void ContactResolver::ResolveContacts(Contact* contacts, uint32_t numContacts, float duration)
		{
			if (numContacts == 0)
				return;
			if (!IsValid())
				return;

			// 准备碰撞数据
			PrepareContacts(contacts, numContacts, duration);
			// 处理碰撞相交
			AdjustPositions(contacts, numContacts, duration);
			// 处理碰撞速度变化
			AdjustVelocities(contacts, numContacts, duration);
		}

		void ContactResolver::PrepareContacts(Contact* contacts, uint32_t numContacts, float duration)
		{
			Contact* lastContact = contacts + numContacts;
			for (Contact* contact = contacts; contact < lastContact; contact++)
			{
				contact->UpdateInternalDatas(duration);
			}
		}

		void ContactResolver::AdjustPositions(Contact* contacts, uint32_t numContacts, float duration)
		{
			// 沿直线移动距离
			Vector3 linearChange[2];
			// 转动角度
			Vector3 angularChange[2];
			// 最大相交深度
			float maxPenetration;

			// 碰撞点的位置变化量(在这里提前声明，避免后面在循环中声明变量，然后在栈上频繁创建和销毁)
			Vector3 deltaPosition;
			// 当前处理的碰撞索引
			uint32_t idx;

			mCurPositionIterations = 0;
			while (mCurPositionIterations < mMaxPositionIterations)
			{
				idx = numContacts;
				maxPenetration = mPositionEpsilon;
				// 找到相交最深的碰撞
				for (uint32_t i = 0; i < numContacts; i++)
				{
					if (contacts[i].mPenetration > maxPenetration)
					{
						maxPenetration = contacts[i].mPenetration;
						idx = i;
					}
				}

				// 说明没有需要处理的相交了，直接退出
				if (idx == numContacts)
					break;

				// 唤醒可能处于休眠状态的刚体
				contacts[idx].MatchAwakeState();

				// 处理相交，并获取相交处理信息，这里会实际改变两个刚体的位置和旋转状态
				contacts[idx].ResolvePenetration(linearChange, angularChange, maxPenetration);

				// 然后遍历所有的碰撞，查询本次相交处理可能会影响到的碰撞
				// 因为本次相交处理所改变的刚体可能同时还有其它相交，所以可能还会影响其它碰撞中的相交状态
				for (uint32_t i = 0; i < numContacts; i++)
				{
					// j代表当前遍历到的碰撞中的刚体索引
					for (uint32_t j = 0; j < 2; j++)
					{
						// 如果当前遍历到的碰撞对象存在刚体才继续
						if (contacts[i].mRigidBodies[j])
						{
							// k代表本次相交处理所改变的碰撞中的刚体索引
							for (uint32_t k = 0; k < 2; k++)
							{
								// 如果当前遍历到的碰撞中的刚体与本次相交处理所改变的碰撞中的刚体相同
								// 那么就需要更新当前遍历到的这个碰撞的相交信息，具体来说就是相交深度mPenetration
								// 因为对一次碰撞中的相交处理所带来的刚体位置旋转状态变化，可能会影响多个碰撞的相交深度
								if (contacts[i].mRigidBodies[j] == contacts[idx].mRigidBodies[k])
								{
									// 计算当前这个刚体碰撞点的位置变化，由刚体的线性运动和角运动一起产生
									deltaPosition = linearChange[k] + Math::Cross(angularChange[k], contacts[i].mRelativeContactPosition[j]);
									// 把这个位置变化投影到碰撞法线方向上，得到在碰撞法线上的变化情况，也就是对相交深度的影响
									// 然后把这个变化量添加到当前遍历到的这个碰撞上，也就是本次相交处理对当前这个碰撞的相交深度所产生的影响
									// 这里是mPenetration - delta，由于mPenetration是从碰撞中的第一个刚体的视角来看的
									// 所以要判断符号，如果当前这个刚体在当前这个碰撞中处于第一个位置就是减，但是如果处于第二个位置就负负得正需要加
									contacts[i].mPenetration += Math::Dot(deltaPosition, contacts[i].mContactNormal) * (j ? 1.0f : -1.0f);
								}
							}
						}
					}
				}

				// 更新完当前碰撞的相交处理带来的变化后，迭代次数加一，然后继续寻找更新后相交深度最深的碰撞，继续做相交处理
				mCurPositionIterations++;
			}
		}

		void ContactResolver::AdjustVelocities(Contact* contacts, uint32_t numContacts, float duration)
		{
			// 线性速度改变量
			Vector3 linearVelocityChange[2];
			// 角速度改变量
			Vector3 angularVelocityChange[2];
			// 最大速率
			float maxSpeed;

			// 碰撞点的闭合速度变化量(在这里提前声明，避免后面在循环中声明变量，然后在栈上频繁创建和销毁)
			Vector3 deltaVelocity;
			// 当前处理的碰撞索引
			uint32_t idx;

			mCurVelocityIterations = 0;
			while (mCurVelocityIterations < mMaxVelocityIterations)
			{
				idx = numContacts;
				maxSpeed = mVelocityEpsilon;
				// 找到速度改变最大的碰撞，优先处理，因为速度变化越大对整体效果影响也越大
				for (uint32_t i = 0; i < numContacts; i++)
				{
					if (contacts[i].mDesiredDeltaVelocity > maxSpeed)
					{
						maxSpeed = contacts[i].mDesiredDeltaVelocity;
						idx = i;
					}
				}

				// 说明没有需要处理的速度变化了，直接退出
				if (idx == numContacts)
					break;

				// 唤醒可能处于休眠状态的刚体
				contacts[idx].MatchAwakeState();

				// 处理速度变化，并获取速度变化信息，这里会实际改变两个刚体的速度状态
				contacts[idx].ResolveVelocityChange(linearVelocityChange, angularVelocityChange);

				// 这里的循环逻辑同AdjustPositions
				for (uint32_t i = 0; i < numContacts; i++)
				{
					for (uint32_t j = 0; j < 2; j++)
					{
						if (contacts[i].mRigidBodies[j])
						{
							for (uint32_t k = 0; k < 2; k++)
							{
								if (contacts[i].mRigidBodies[j] == contacts[idx].mRigidBodies[k])
								{
									// 结合线性速度改变量和角速度改变量，计算碰撞点的闭合速度变化量
									deltaVelocity = linearVelocityChange[k] + Math::Cross(angularVelocityChange[k], contacts[i].mRelativeContactPosition[j]);
									// 将变化量转换到碰撞空间，然后加到位于碰撞空间的闭合速度上
									contacts[i].mContactVelocity += Math::Transpose(contacts[i].mContactToWorld) * deltaVelocity * (j ? -1.0f : 1.0f);
									// 更新期望速度变化量
									contacts[i].UpdateDesiredDeltaVelocity(duration);
								}
							}
						}
					}
				}

				// 迭代次数加一，继续寻找速度变化最大的碰撞，继续做速度变化处理
				mCurVelocityIterations++;
			}
		}
	}
}