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

            // ����뾶��������ľ��룬˵��һ������ȫ��������һ����
            if (radiusDiff * radiusDiff >= distance)
            {
                // ֱ���ð뾶�������Ϊ��Χ��
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
            // �����򲿷��ཻ����ȫ���ཻ
            else
            {
                distance = sqrt(distance);
                mRadius = (distance + bs1.mRadius + bs2.mRadius) * 0.5f;

                // �µ�����λ���Ǵ�ԭ����һ�����ĳ���������һ�������ĵķ����ƶ�һ�ξ���
                // ����Ϊ����뾶��ȥ������뾶
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
            // ֱ�ӹ���һ����Χ����������°�Χ��
            BoundingSphere tmpSphere(*this, other);
            // �ð뾶ƽ������������ʾ��������(ĳЩ�㷨���ñ��������Goldsmith-Salmon�㷨)
            return tmpSphere.mRadius * tmpSphere.mRadius - mRadius * mRadius;
        }
    }
}