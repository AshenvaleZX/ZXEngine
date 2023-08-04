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

		void Contact::MatchAwakeState()
		{
			// 只有两个碰撞对象都是刚体时才可能需要更新(前面的代码会保证这个函数调用时，如果其中一个对象是非刚体，一定在第二个位置)
			// 如果其中一个对象是非刚体(地面/墙壁)，那么另一个对象是否Awake都行，因为只有在一个对象Awake一个对象Sleep的时候才需要唤醒Sleep的那个
			if (!mRigidBodies[1])
				return;

			bool awake0 = mRigidBodies[0]->GetAwake();
			bool awake1 = mRigidBodies[1]->GetAwake();

			// 如果一个Awake另一个Sleep，唤醒Sleep的那个
			if (awake0 ^ awake1)
			{
				if (awake0)
					mRigidBodies[1]->SetAwake(true);
				else
					mRigidBodies[0]->SetAwake(true);
			}
		}

		void Contact::ResolvePenetration(Vector3 linearChange[2], Vector3 angularChange[2], float penetration)
		{
			// 两个刚体需要通过线性运动来移动的距离
			float linearMove[2]  = { 0.0f, 0.0f };
			// 两个刚体需要通过旋转运动来移动的距离
			float angularMove[2] = { 0.0f, 0.0f };

			// 总惯性大小
			float totalInertia = 0.0f;
			// 每单位冲量导致的两个刚体的线性运动所带来的在碰撞点的速度变化(这里用来衡量惯性大小)
			float linearInertia[2]  = { 0.0f, 0.0f };
			// 每单位冲量导致的两个刚体的旋转所带来的在碰撞点的速度变化(这里用来衡量惯性大小)
			float angularInertia[2] = { 0.0f, 0.0f };

			for (uint32_t i = 0; i < 2; i++)
			{
				if (mRigidBodies[i])
				{
					// 世界坐标系下的惯性张量的逆矩阵表达
					Matrix3 inverseInertiaTensor;
					mRigidBodies[i]->GetInverseInertiaTensorWorld(inverseInertiaTensor);

					// 这一行计算的是单位线性冲量(Linear Impulse)造成的角冲量(Angular Impulse)
					// 
					// 冲量是碰撞前后动量的变化
					// 线性冲量公式为: Jf = m * (v - v')
					// 其中m为物体质量，v和v'为碰撞前后速度
					// 角冲量公式为: Jt = I * (w - w')
					// 其中I为物体惯性张量，w和w'为碰撞前后角速度
					// 线性冲量和角冲量的关系为: Jt = r x Jf
					// 其中r是力的作用点到旋转轴的向量
					// 
					// 由于线性冲量的方向就是碰撞法线的方向，所以碰撞法线可以当作是单位线性冲量
					// 然后根据上面的公式计算得到在当前作用力下，单位线性冲量对应的单位角冲量
					Vector3 velocityPerUnitImpulseWorld = Math::Cross(mRelativeContactPosition[i], mContactNormal);
					// 然后角冲量除以惯性张量(即惯性张量的逆矩阵*角冲量)得到角速度的变化量
					// 这里得到的就是单位线性冲量带来的角速度变化
					velocityPerUnitImpulseWorld = inverseInertiaTensor * velocityPerUnitImpulseWorld;
					// 速度 = 角速度 x 作用点到旋转轴的向量
					// 所以再次叉乘作用点到旋转轴的向量，得到了单位线性冲量带来的旋转导致的速度变化
					velocityPerUnitImpulseWorld = Math::Cross(velocityPerUnitImpulseWorld, mRelativeContactPosition[i]);

					// 目前计算出来的这个速度在世界坐标系，需要转换到碰撞坐标系
					// Vector3 velocityPerUnitImpulseContact = Math::Inverse(mContactToWorld) * velocityPerUnitImpulseWorld;
					// 
					// 然后这里我们只关心碰撞法线上的速度，所以只取x分量的值
					// float angularComponent = velocityPerUnitImpulseContact.x;
					// 
					// 然后因为只取了最终结果的x分量，而x分量实际上就是Math::Inverse(mContactToWorld)矩阵的第一行和velocityPerUnitImpulseWorld点乘
					// 所以上面的2行代码可以精简为下面这一行
					// 最后算出来的这个值的意思就是：由单位线性冲量造成的旋转所导致的碰撞点的速度变化量在碰撞法线上的分量
					// 这里用这个量来作为一种衡量旋转惯性大小的数值，值越大惯性越小
					angularInertia[i] = Math::Dot(velocityPerUnitImpulseWorld, mContactNormal);

					// 然后再计算单位冲量带来的线性运动的速度变化
					// 单位冲量直接除以质量就可以得到速度变化量
					// 然后因为冲量方向就是碰撞法线方向，所以得到的速度变化量也不需要取分量
					// 所以直接用1除以质量即可，也就是质量倒数
					// 这个值的意思就是：由单位冲量造成的线性速度变化量(在碰撞法线上的分量)
					// 这个数值也是可以用来表示惯性大小的，值越大线性运动惯性越小
					linearInertia[i] = mRigidBodies[i]->GetInverseMass();

					// 然后叠加到总的惯性上
					totalInertia += linearInertia[i] + angularInertia[i];
				}
			}

			for (uint32_t i = 0; i < 2; i++)
			{
				if (mRigidBodies[i])
				{
					// 两个刚体的分离操作同时包含了线性运动和角运动
					// 分离操作所需的线性运动和角运动的量与惯性的倒数成正比
					// 整个分离操作一共需要让两个碰撞对象之间互相远离penetration大小的距离
					// 这个距离一共分为四部分，即两个刚体各自的线性运动和角运动
					// 如果其中一个对象为非刚体，则只有2部分，即刚体对象的线性运动和角运动
					// 最终这4个(或者2个)运动造成的远离距离加起来等于penetration
					// 这里判断一下方向，两个刚体移动方向是相反的，互相远离(方向是以第一个对象的视角来看)
					float sign = (i == 0) ? 1.0f : -1.0f;
					// 第i个刚体需要通过线性运动来远离的距离
					linearMove[i] = sign * penetration * (linearInertia[i] / totalInertia);
					// 第i个刚体需要通过角运动来远离的距离
					angularMove[i] = sign * penetration * (angularInertia[i] / totalInertia);
					
					// 这里需要限制角运动的距离，不要太大了，不然分离的时候可能会出现过度旋转
					// 如果出现了过度旋转，可能会导致两个对象无法分离，或者原碰撞点分离了但是又在另一边产生了新碰撞点
					// 所以要限制分离运动中旋转所占的比例
					// 限制方式可以有很多种，比如限制固定移动距离，或者固定旋转角度
					// 这里通过物体的大小来计算限制移动的距离，越大的物体允许旋转移动的距离也越大
					// 因为移动相同距离的话，越大的物体实际需要旋转的角度越小
					float limit = 0.2f * mRelativeContactPosition[i].GetMagnitude();
					// 如果角运动的距离超过了限制，就需要重新分配
					if (fabsf(angularMove[i]) > limit)
					{
						// 先计算总移动距离，这个值不变
						float totalMove = linearMove[i] + angularMove[i];

						// 然后把角运动移动的距离调整为限制的最大距离，同时注意保持符号不变
						if (angularMove[i] >= 0)
							angularMove[i] = limit;
						else 
							angularMove[i] = -limit;

						// 然后重新计算线性运动距离
						linearMove[i] = totalMove - angularMove[i];
					}

					// 角运动最终是通过旋转来实现分离的，我们已经知道了需要通过角运动来远离的距离
					// 然后需要反推需要旋转多少角度，才能达到这个移动距离
					// 如果需要旋转移动的距离非常小，就直接忽略掉
					if (angularMove[i] < 0.0001f)
					{
						angularChange[i].Clear();
					}
					else
					{
						// 同上一个for循环中的解释，这是在当前作用力下，单位线性冲量对应的单位角冲量
						Vector3 angularImpulsePerUnitLinearImpulse = Math::Cross(mRelativeContactPosition[i], mContactNormal);
						// 同上，单位线性冲量带来的角速度变化
						Vector3 angularVelocityPerLinearImpulse = mRigidBodies[i]->GetInverseInertiaTensorWorld() * angularImpulsePerUnitLinearImpulse;
						// 上面解释过这个angularInertia的意思是：由单位线性冲量造成的旋转所导致的碰撞点的速度变化量在碰撞法线上的分量
						// 那么用“单位线性冲量带来的角速度变化”除以“单位线性冲量造成的旋转所导致的碰撞点的速度变化量在碰撞法线上的分量”
						// 就得到了“沿着碰撞法线方向每变化一个单位速度需要变化的角速度”
						// 如果 角速度 = n线速度，那么两边同时乘以时间t等式依旧成立，即 角度 = n距离
						// 所以也就相当于得到了“沿着碰撞法线每移动一个单位距离需要旋转的角度”
						Vector3 angularPerMove = angularVelocityPerLinearImpulse / angularInertia[i];
						// 然后乘以需要移动的距离，就得到了需要旋转的角度
						angularChange[i] = angularPerMove * angularMove[i];
					}

					// 线性运动的移动就直接是需要的距离乘以碰撞法线即可
					linearChange[i] = mContactNormal * linearMove[i];

					// 调整刚体位置(通过线性运动远离另一个碰撞对象)
					Vector3 pos;
					mRigidBodies[i]->GetPosition(pos);
					pos += linearChange[i];
					mRigidBodies[i]->SetPosition(pos);

					// 调整刚体旋转(通过角运动远离另一个碰撞对象)
					Quaternion q;
					mRigidBodies[i]->GetRotation(q);
					q.RotateByVector(angularChange[i]);
					mRigidBodies[i]->SetRotation(q);

					// 如果刚体处于Sleep状态，手动调用接口更新相关数据
					// 否则此函数无法更新这个刚体，导致下一帧还会有相同的相交深度(penetration)
					if (!mRigidBodies[i]->GetAwake())
						mRigidBodies[i]->CalculateDerivedData();
				}
			}
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