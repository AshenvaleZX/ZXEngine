#include "BoundingSphere.h"

namespace ZXEngine
{
	namespace PhysZ
	{
		BoundingSphere::BoundingSphere(const Vector3& center, float radius) : 
			mCenter(center), 
			mRadius(radius) 
		{};

		BoundingSphere::BoundingSphere(const BoundingSphere& bs1, const BoundingSphere& bs2)
		{
            Vector3 centreOffset = bs2.mCenter - bs1.mCenter;
            float distance = centreOffset.GetMagnitudeSquared();
            float radiusDiff = bs2.mRadius - bs1.mRadius;

            // 如果半径差大于球心距离，说明一个球完全覆盖了另一个球
            if (radiusDiff * radiusDiff >= distance)
            {
                // 直接用半径大的球作为包围球
                if (bs1.mRadius > bs2.mRadius)
                {
                    mCenter = bs1.mCenter;
                    mRadius = bs1.mRadius;
                }
                else
                {
                    mCenter = bs2.mCenter;
                    mRadius = bs2.mRadius;
                }
            }
            // 两个球部分相交或完全不相交
            else
            {
                distance = sqrt(distance);
                mRadius = (distance + bs1.mRadius + bs2.mRadius) * 0.5f;

                // 新的球心位置是从原来的一个球心出发，向另一个球球心的方向移动一段距离
                // 距离为新球半径减去出发球半径
                if (distance > 0)
                    mCenter = bs1.mCenter + (centreOffset / distance) * (mRadius - bs1.mRadius);
                else
                    mCenter = bs1.mCenter;
            }
		}

        float BoundingSphere::GetVolume() const
        {
            return 1.33333333333f * Math::PI * mRadius * mRadius * mRadius;
        }

        bool BoundingSphere::IsOverlapWith(const BoundingSphere& other) const
        {
            return (mCenter - other.mCenter).GetMagnitudeSquared() <= (mRadius + other.mRadius) * (mRadius + other.mRadius);
        }

        float BoundingSphere::GetGrowth(const BoundingSphere& other) const
        {
            // 直接构造一个包围这两个球的新包围球
            BoundingSphere tmpSphere(*this, other);
            // 用半径平方的增长来表示增长幅度(某些算法会用表面积，如Goldsmith-Salmon算法)
            return tmpSphere.mRadius * tmpSphere.mRadius - mRadius * mRadius;
        }
    }
}