#include "Contact.h"

namespace ZXEngine
{
	namespace PhysZ
	{
		void Contact::SetRigidBodies(RigidBody* rigidBody1, RigidBody* rigidBody2)
		{
			mRigidBodies[0] = rigidBody1;
			mRigidBodies[1] = rigidBody2;
		}

		void Contact::GenerateOrthogonalBasis()
		{
			// 默认碰撞法线为碰撞坐标系的X轴，计算碰撞坐标系的YZ轴
			// 这里在计算的时候要特别注意左右手坐标系的问题！！！
			Vector3 axisY, axisZ;

			// 先计算Y轴再计算Z轴，在只确定了X轴的情况下YZ轴有无数种可能性，随便一种都行
			// 所以可以用任意一个轴和碰撞法线叉乘得到碰撞坐标系Y轴，这里选择世界坐标系的X轴或者Z轴
			// 为了避免碰撞法线刚好和世界坐标系的X轴或者Z轴重合，这里要判断碰撞法线离哪个轴更近
			// 如果碰撞法线的x分量比z分量大，那么碰撞法线离世界坐标系的X轴更近，用世界坐标系的Z轴来叉乘得到碰撞坐标系的Y轴
			if (fabsf(mContactNormal.x) > fabsf(mContactNormal.z))
			{
				// 即使是两个单位向量叉乘，如果不是垂直关系，结果也不会是单位向量
				// 所以这里提前计算归一化需要的数据，在后面的简化算法里使用
				const float s = 1.0f / sqrtf(mContactNormal.x * mContactNormal.x + mContactNormal.y * mContactNormal.y);

				// 以下写法是简化后的计算，等价于axisY = Math::Cross(mContactNormal, Vector3(0.0f, 0.0f, 1.0f)).GetNormalized();
				axisY.x =  mContactNormal.y * s;
				axisY.y = -mContactNormal.x * s;
				axisY.z =  0.0f;

				// 以下写法是简化后的计算，等价于axisZ = Math::Cross(axisY, ontactNormal);
				axisZ.x =  axisY.y * mContactNormal.z;
				axisZ.y = -axisY.x * mContactNormal.z;
				axisZ.z =  axisY.x * mContactNormal.y - axisY.y * mContactNormal.x;
			}
			// 碰撞法线离世界坐标系Z轴更近，用世界坐标系X轴叉乘得到碰撞坐标系Y轴
			else
			{
				const float s = 1.0f / sqrtf(mContactNormal.z * mContactNormal.z + mContactNormal.y * mContactNormal.y);

				// 以下写法是简化后的计算，等价于axisY = Math::Cross(Vector3(1.0f, 0.0f, 0.0f), mContactNormal).GetNormalized();
				axisY.x =  0.0f;
				axisY.y = -mContactNormal.z * s;
				axisY.z =  mContactNormal.y * s;

				// 以下写法是简化后的计算，等价于axisZ = Math::Cross(ontactNormal, axisY);
				axisZ.x =  mContactNormal.y * axisY.z - mContactNormal.z * axisY.y;
				axisZ.y = -mContactNormal.x * axisY.z;
				axisZ.z =  mContactNormal.x * axisY.y;
			}

			// 构造坐标系旋转矩阵
			mContactToWorld = Matrix3(mContactNormal.x, axisY.x, axisZ.x,
			                          mContactNormal.y, axisY.y, axisZ.y,
				                      mContactNormal.z, axisY.z, axisZ.z);
		}
	}
}