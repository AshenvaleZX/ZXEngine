#include "CollisionPrimitive.h"

namespace ZXEngine
{
	namespace PhysZ
	{
		float CollisionBox::GetHalfProjectedLength(const Vector3& axis) const
		{
			// �Ȱ��������ռ�任��Box�ľֲ��ռ�(��������ת����)
			Vector3 axisProjected = Math::Transpose(Matrix3(mTransform)) * axis;
			// ��mHalfSize�ŵ�ͬһ��������
			axisProjected.Absolutize();
			// Ȼ�����Box��������ϵ�ͶӰ����
			return Math::Dot(mHalfSize, axisProjected);
		}
	}
}