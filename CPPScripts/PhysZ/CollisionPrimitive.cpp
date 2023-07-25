#include "CollisionPrimitive.h"

namespace ZXEngine
{
	namespace PhysZ
	{
		float CollisionBox::GetHalfProjectedLength(const Vector3& axis) const
		{
			// 先把轴从世界空间变换到Box的局部空间(仅考虑旋转即可)
			Vector3 axisProjected = Math::Transpose(Matrix3(mTransform)) * axis;
			// 和mHalfSize放到同一个象限内
			axisProjected.Absolutize();
			// 然后计算Box在这个轴上的投影长度
			return Math::Dot(mHalfSize, axisProjected);
		}
	}
}