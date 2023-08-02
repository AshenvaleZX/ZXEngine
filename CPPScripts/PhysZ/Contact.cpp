#include "Contact.h"
#include "RigidBody.h"

namespace ZXEngine
{
	namespace PhysZ
	{
		void Contact::SetRigidBodies(RigidBody* rigidBody1, RigidBody* rigidBody2)
		{
			mRigidBodies[0] = rigidBody1;
			mRigidBodies[1] = rigidBody2;
		}

		void Contact::SwapRigidBodies()
		{
			mContactNormal *= -1.0f;

			RigidBody* temp = mRigidBodies[0];
			mRigidBodies[0] = mRigidBodies[1];
			mRigidBodies[1] = temp;
		}

		void Contact::UpdateInternalDatas(float duration)
		{
			// 要计算第一个刚体的碰撞反馈，所以第一个刚体必须存在，第二个刚体可以不存在(墙壁/地面等不可移动对象)
			if (!mRigidBodies[0])
			{
				if (!mRigidBodies[1])
				{
					Debug::LogError("Both rigid bodies are null!");
					return;
				}
				else
				{
					SwapRigidBodies();
				}
			}

			// 更新碰撞坐标系到世界坐标系的旋转矩阵
			UpdateOrthogonalBasis();

			// 计算碰撞点相对于两个刚体的位置
			mRelativeContactPosition[0] = mContactPoint - mRigidBodies[0]->GetPosition();
			if (mRigidBodies[1])
				mRelativeContactPosition[1] = mContactPoint - mRigidBodies[1]->GetPosition();

			// 两个对象在碰撞点的相对速度
			mContactVelocity = CalculateLocalVelocity(0, duration);
			// 如果第二个对象也是刚体(不是墙壁/地面等不可移动对象)，则减去第二个对象在碰撞点的相对速度，得到两个移动中的对象之间的相对速度
			if (mRigidBodies[1])
				mContactVelocity -= CalculateLocalVelocity(1, duration);

			// 计算当前碰撞所产生的期望速度变化量
			UpdateDesiredDeltaVelocity(duration);
		}

		void Contact::UpdateOrthogonalBasis()
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

		void Contact::UpdateDesiredDeltaVelocity(float duration)
		{
			// 当前帧加速度导致的沿着碰撞法线的闭合速度变化
			float deltaVelocity = 0.0f;

			if (mRigidBodies[0]->GetAwake())
				deltaVelocity += Math::Dot(mRigidBodies[0]->GetLastAcceleration() * duration, mContactNormal);
			if (mRigidBodies[1] && mRigidBodies[1]->GetAwake())
				deltaVelocity -= Math::Dot(mRigidBodies[1]->GetLastAcceleration() * duration, mContactNormal);

			// 如果闭合速度太小了，恢复系数就按0处理，即碰撞后闭合速度为0，两物体之间无相对速度，贴合到一起
			float restitution = fabsf(mContactVelocity.x) < 0.25f ? 0.0f : mRestitution;

			// 计算碰撞法线方向上的期望速度变化
			// 如果不考虑闭合速度的加速度(或闭合速度的加速度为0)
			// 那么恢复系数为0时，期望的结果是两物体挨在一起，闭合速度变化就是原闭合速度的负数，也就是经过这个变化后闭合速度为0
			// 如果恢复系数为1时，期望的结果是两物体无相对速度的损失，按速度大小不变，但是方向相反的方式运动，也就是闭合速度变化为2倍反向原闭合速度
			// 考虑加速度之后，由恢复系数产生的碰撞后闭合速度(分离速度)就需要加上加速度的带来的增量，相反计算闭合速度变化量的时候就要减掉来抵消
			mDesiredDeltaVelocity = -mContactVelocity.x - restitution * (mContactVelocity.x - deltaVelocity);
		}

		Vector3 Contact::CalculateLocalVelocity(uint32_t index, float duration)
		{
			auto rigidBody = mRigidBodies[index];

			// 先计算碰撞点由于旋转产生的速度
			Vector3 velocity = Math::Cross(rigidBody->GetAngularVelocity(), mRelativeContactPosition[index]);
			// 再加上刚体的线性速度得到碰撞点的当前速度(世界坐标系)
			velocity += rigidBody->GetVelocity();

			// 将速度转换到碰撞坐标系(非碰撞法线上的速度分量体现了对象间彼此的移动速度，计算摩擦力的时候需要用)
			Vector3 contactVelocity = mContactToWorld * velocity;

			// 在不考虑反作用力的情况下，加速度带来的速度变化
			Vector3 deltaVelocity = rigidBody->GetLastAcceleration() * duration;
			// 转换到碰撞坐标系
			deltaVelocity = mContactToWorld * deltaVelocity;
			// 去掉碰撞法线上的分量，只保留在碰撞平面上的速度
			deltaVelocity.x = 0.0f;
			// 添加碰撞平面上的速度变化(如果有摩擦力这个速度会在后面被抵消掉)
			contactVelocity += deltaVelocity;

			return contactVelocity;
		}
	}
}