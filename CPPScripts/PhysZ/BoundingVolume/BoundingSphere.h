#pragma once
#include "../../pubh.h"

namespace ZXEngine
{
	namespace PhysZ
	{
		class BoundingSphere
		{
		public:
			BoundingSphere(const Vector3& center, float radius);
			BoundingSphere(const BoundingSphere& bs1, const BoundingSphere& bs2);
			~BoundingSphere() {};

			// 获取包围球体积
			float GetVolume() const;
			// 与另一个包围球是否相交
			bool IsOverlapWith(const BoundingSphere* other) const;
			// 如果这个包围球要把另一个包围球包进去，需要变大多少
			// 返回的数值只是个增长幅度，不一定是具体的什么半径，面积或者体积等有实际意义的数值
			float GetGrowth(const BoundingSphere& other) const;

		private:
			float mRadius;
			Vector3 mCenter;
		};
	}
}