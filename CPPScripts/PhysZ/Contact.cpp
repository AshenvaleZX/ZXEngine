#include "Contact.h"
#include "RigidBody.h"

namespace ZXEngine
{
	namespace PhysZ
	{
		Contact::Contact(RigidBody* rigidBody1, RigidBody* rigidBody2)
		{
			mRigidBodies[0] = rigidBody1;
			mRigidBodies[1] = rigidBody2;
		}

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
					// 或者说这是在当前的这个碰撞点上，对于当前这个刚体，沿着碰撞法线的单位线性冲量对应的角冲量
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

		void Contact::ResolveVelocityChange(Vector3 linearVelocityChange[2], Vector3 angularVelocityChange[2])
		{
			// 获取两个刚体的逆惯性张量
			Matrix3 inverseInertiaTensor[2];
			mRigidBodies[0]->GetInverseInertiaTensorWorld(inverseInertiaTensor[0]);
			if (mRigidBodies[1])
				mRigidBodies[1]->GetInverseInertiaTensorWorld(inverseInertiaTensor[1]);

			// 碰撞空间中的冲量
			Vector3 impulseContact;
			// 之前已经先通过两个碰撞对象当前的速度，加速度，恢复系数等信息计算出来了本次碰撞会产生的期望速度变化
			// 这个期望速度变化是闭合速度的变化，不是直接等于两个碰撞对象速度的变化
			// 这里通过期望的闭合速度变化倒推出需要的冲量，后面再根据这个冲量来计算两个对象的速度变化
			if (Math::Approximately(mFriction, 0.0f))
				impulseContact = CalculateFrictionlessImpulse(inverseInertiaTensor);
			else
				impulseContact = CalculateFrictionImpulse(inverseInertiaTensor);
			
			// 转换到世界空间
			Vector3 impulseWorld = mContactToWorld * impulseContact;

			// 计算角冲量，Jt = r x Jf
			Vector3 angularImpulse = Math::Cross(mRelativeContactPosition[0], impulseWorld);
			// 角冲量公式为: Jt = I * w，其中w为角速度，所以w = I^-1 * Jt
			angularVelocityChange[0] = inverseInertiaTensor[0] * angularImpulse;
			// 线性冲量公式为: J = m * v，其中v为线速度，所以v = J / m
			linearVelocityChange[0] = impulseWorld * mRigidBodies[0]->GetInverseMass();

			// 更新刚体速度
			mRigidBodies[0]->AddVelocity(linearVelocityChange[0]);
			mRigidBodies[0]->AddAngularVelocity(angularVelocityChange[0]);

			// 如果有第二个刚体，就更新第二个刚体的速度
			if (mRigidBodies[1])
			{
				// 对于第二个刚体，冲量的作用是相反的
				Vector3 i2 = Math::Cross(impulseWorld, mRelativeContactPosition[1]);
				angularVelocityChange[1] = inverseInertiaTensor[1] * i2;
				linearVelocityChange[1] = impulseWorld * -mRigidBodies[1]->GetInverseMass();

				mRigidBodies[1]->AddVelocity(linearVelocityChange[1]);
				mRigidBodies[1]->AddAngularVelocity(angularVelocityChange[1]);
			}
		}

		Vector3 Contact::CalculateFrictionImpulse(Matrix3* inverseInertiaTensor)
		{
			// 冲量公式为: J = m * v
			// 所以这里取质量的倒数就直接等于单位冲量产生的线性速度变化值
			float linearVelocity = mRigidBodies[0]->GetInverseMass();

			// 这里用相对位置来构造斜对称矩阵
			// 假设原向量为v0，构造出来的矩阵为m0，那么m0 * v1 = v0 x v1
			Matrix3 impulseToTorque(mRelativeContactPosition[0]);

			// 这里的逻辑和ResolvePenetration函数里的一样，区别在于ResolvePenetration里只计算了碰撞法线方向上的速度，所以用Vector3作为结果
			// 而这里要计算的是三个坐标轴上的速度，所以有3个结果，用Matrix3表示(三个Vector3拼到一起)
			// 
			// 按照之前ResolvePenetration函数里的计算流程，这里应该先分别计算三个轴:
			// Math::Cross(mRelativeContactPosition[i], axisX/axisY/axisZ);
			// 因为当前在实际坐标系下，所以这三个轴实际上就是(1, 0, 0), (0, 1, 0), (0, 0, 1)
			// 
			// 这里可以借助矩阵和向量的乘法规则简化了一下代码，因为把三个轴分开计算的结果，和把三个轴放到一起组成矩阵再一次性计算的结果是一样的
			// 所以上面的3次向量叉乘可以简化为 斜对称矩阵 * I (XYZ三个轴向量拼起来就是个单位矩阵)
			// 结果还是那个斜对称矩阵，所以这里实际上比起ResolvePenetration里的计算就直接跳过了一步
			// 此时这个斜对称矩阵的第一列，第二列和第三列分别对应了 mRelativeContactPosition[i] 与 axisX/axisY/axisZ 的叉乘结果
			// 
			// 本来第一步 Math::Cross(mRelativeContactPosition[i], mContactNormal); 的结果得到了沿着碰撞法线的单位线性冲量对应的角冲量
			// 这里的impulseToTorque矩阵实际上直接就等于了沿着三个基坐标轴的单位线性冲量对应的角冲量
			// 
			// 然后这里就可以直接开始第二步，用逆惯性张量矩阵*角冲量得到一个角速度，即 w = I^-1 * Jt
			// 在这里的意思就是，沿着三个基坐标轴的单位线性冲量带来的三个轴上的角速度变化
			// x轴上的角速度变化在矩阵第一列，y轴在第二列，z轴在第三列
			// 所以第二步应该是这样的：
			// Matrix3 deltaVelocityWorld = inverseInertiaTensor[0] * impulseToTorque;
			// 
			// 然后第三步，把角速度转换成线速度，这里同样是一次性算三个轴上的线速度
			// 这里应该是三个轴上的角速度分别叉乘 mRelativeContactPosition[i]
			// 三个轴上的角速度变化量已经在上一步的计算结果中了，即矩阵的第一二三列数据
			// 这里直接 * impulseToTorque得到的矩阵的第一行就是x轴上的角速度变化量(即上一步算出来的矩阵的第一列)与mRelativeContactPosition[i]叉乘的结果
			// 第二，三行分别是y轴，z轴上的角速度变化量与mRelativeContactPosition[i]叉乘的结果
			// 所以这里的结果意思是，沿着三个基坐标轴的单位线性冲量带来的刚体旋转导致的当前碰撞点处在三个轴上的线性速度变化
			// 第三步应该是这样的：
			// deltaVelocityWorld *= impulseToTorque;
			// 
			// 最终代码等于：
			// Matrix3 deltaVelocityWorld = inverseInertiaTensor[0] * impulseToTorque * impulseToTorque;
			// 
			// 但是这个和Cyclone引擎里的代码不一样，并且运行出来会产生错误碰撞效果，所以这里最终实际代码还是参照了Cyclone引擎里的
			// 然而我并没有理解Cyclone引擎里的代码为什么是这样的
			// 本来我以为惯性张量矩阵是对称矩阵(设为A)，impulseToTorque是斜对称矩阵(设为B)，那么由矩阵性质可得 A * B = -(B * A)
			// 所以Cyclone引擎的写法和我的写法是等价的，但是实际这里的惯性张量矩阵是被转换到了世界坐标系下的，所以不再是对称矩阵了
			// 我的写法就不等价于Cyclone引擎里的了，而且我的写法错了，现在还没想通为什么要写成Cyclone引擎里的这个样子
			Matrix3 deltaVelocityWorld = impulseToTorque * inverseInertiaTensor[0] * impulseToTorque * -1.0f;

			// 
			// 这里在Cyclone引擎里本来是这样写的：
			// Matrix3 deltaVelocityWorld = impulseToTorque * inverseInertiaTensor[0];
			// 然后最后计算完后又 * -1，这里我改变了一下乘法顺序，就不用*-1了，而且和ResolvePenetration里的过程也更一致
			// 这里之所以可以这样是因为impulseToTorque是一个斜对称矩阵，而惯性张量矩阵又是对称矩阵(实际上很多时候是对角矩阵)，惯性张量矩阵的逆矩阵也肯定是对称矩阵
			// 这种情况下，假设斜对称矩阵为A，对称矩阵为B，那么 A * B = -(B * A)，所以Cyclone引擎里的实现和我这里的实现实际上是一样的
			//Matrix3 deltaVelocityWorld = inverseInertiaTensor[0] * impulseToTorque;


			// 如果有第二个刚体，那么就把第二个刚体的速度变化也加进来
			if (mRigidBodies[1])
			{
				impulseToTorque = Matrix3(mRelativeContactPosition[1]);

				Matrix3 deltaVelocityWorld2 = impulseToTorque * inverseInertiaTensor[1] * impulseToTorque * -1.0f;

				// 把两个刚体的速度变化矩阵相加
				deltaVelocityWorld += deltaVelocityWorld2;

				// 把两个刚体的质量相加
				linearVelocity += mRigidBodies[1]->GetInverseMass();
			}

			// 把速度变化矩阵转换到碰撞空间
			Matrix3 deltaVelocityContact = Math::Transpose(mContactToWorld) * deltaVelocityWorld * mContactToWorld;

			// 上面算的都是旋转带来的线性速度变化，这里再加上线性运动带来的线性速度变化
			// 这里线性运动本身的速度变化同样也用矩阵表达，即 Matrix3(linearVelocity)
			// 意思是，沿着三个基坐标轴的单位线性冲量带来的刚体线性运动导致的当前碰撞点处在三个轴上的线性速度变化
			// 然后叠加到前面旋转导致的线性速度变化上，得到总的速度变化量
			// 
			// 此时这个矩阵的意义是，沿着三个基坐标轴的单位冲量带来的线性运动和旋转结合起来导致的当前碰撞点在三个轴上的速度变化
			// 
			// 如果按行来看待，第一行就是沿着x轴的单位冲量带来的碰撞点速度变化在xyz轴上的分量，因为有旋转所以x轴的冲量不止影响x轴上的速度
			// 第二，三行就是沿着y，z轴的单位冲量带来的碰撞点速度变化在xyz轴上的分量
			// 
			// 如果按列来看待，第一列就是沿着xyz三个轴的单位冲量分别能带来的x轴上的速度变化量
			// 第二，三列就是沿着xyz三个轴的单位冲量分别能带来的y，z轴上的速度变化量
			deltaVelocityContact += Matrix3(linearVelocity);

			// 通过计算速度变化矩阵的逆矩阵，得到单位速度变化所需要的冲量
			// 这个矩阵的数值含义：
			// 第一行第一列，在x轴上每变化一个单位速度所需要的x轴上的冲量大小
			// 第一行第二列，在y轴上每变化一个单位速度所需要的x轴上的冲量大小
			// 第一行第三列，在z轴上每变化一个单位速度所需要的x轴上的冲量大小
			// 第二行第一列，在x轴上每变化一个单位速度所需要的y轴上的冲量大小
			// 第二行第二列，在y轴上每变化一个单位速度所需要的y轴上的冲量大小
			// 以此类推......
			Matrix3 impulsePerUnitVelocity = Math::Inverse(deltaVelocityContact);

			// 这是两个对象碰撞时要变化的速度(碰撞空间)
			// x方向是碰撞法线方向，变化速度就是本次碰撞期望产生的，在碰撞法线上的变化速度
			// y和z是会产生摩擦力的平面，这两个方向上的速度会因为摩擦力抵消掉(这一次碰撞可能不会完全抵消完)
			Vector3 velocityToRemove(mDesiredDeltaVelocity, -mContactVelocity.y, -mContactVelocity.z);

			// 计算移除上面那个速度所需要的冲量(碰撞空间)
			// 这里按照(矩阵m * 向量v)的计算规则，结果中的x分量 = m00 * x + m01 * y + m02 * z
			// 按照上面解释的此矩阵和此向量的含义，结果中的x分量意义就是要产生速度v所需要的冲量在x轴上的分量大小
			// 整个结果向量的含义就是要产生速度v所需要的冲量大小
			Vector3 impulseContact = impulsePerUnitVelocity * velocityToRemove;

			// 这是产生上面那个速度所需要的冲量在yz平面上的大小
			float planarImpulse = sqrtf(impulseContact.y * impulseContact.y + impulseContact.z * impulseContact.z);

			// 这里这个判断的意思是这个冲量会不会大于摩擦力可以提供的冲量大小
			// 
			// 这个比较的依据来自摩擦力公式: Ff = u * Fn，其中Ff是摩擦力大小，u是摩擦系数，Fn是正压力大小(和摩擦平面垂直)
			// 所以摩擦力和正压力大小成正比，系数为u，而冲量公式为: J = F * dt，其中J是冲量大小，F是力大小，dt是力的作用时间
			// 这里dt是一样的，所以这里的碰撞产生的冲量在x轴(碰撞法线)上的分量大小也就和yz平面(摩擦平面)上的分量大小成正比，系数为u
			// 
			// 如果不大于，那么有点类似静摩擦，作用力和反作用力相等，摩擦力(yz平面上的反作用力)会足够大
			// 也就足够让这次碰撞后yz平面上的相对速度被完全移除掉，所以直接返回刚刚计算的产生velocityToRemove速度的冲量即可
			// 
			// 否则相当于是动摩擦，会减小yz平面上的相对速度，此时重新计算冲量大小
			// 这个冲量在yz平面上的大小就是动摩擦力(反作用力)产生的冲量，不足以完全移除yz平面的相对速度，而是使其减小
			// 进而模拟了摩擦力在碰撞法线的切平面上产生的阻力效果
			if (planarImpulse > impulseContact.x * mFriction)
			{
				// 摩擦力足够大的情况下，yz平面上的冲量就直接是移除yz平面所有速度的冲量
				// 但是在这里摩擦力不够大的情况下，yz平面上的冲量就只能是x轴方向上冲量的u倍
				// 注意是yz平面上的冲量是x轴上冲量的u倍，不等于y轴和z轴上的冲量是x轴的u倍
				// 所以这里先算出单位冲量在y和z轴上的分量
				impulseContact.y /= planarImpulse;
				impulseContact.z /= planarImpulse;
				
				// deltaVelocityContact矩阵的意思是沿着三个基坐标轴的单位冲量带来的线性运动和旋转结合起来导致的当前碰撞点在三个轴上的速度变化
				// 这里取到的向量x分量代表沿基坐标系x轴的单位冲量带来的在x轴上的速度变化量
				// y分量代表沿基坐标系y轴的单位冲量带来的在x轴上的速度变化
				// z分量代表沿基坐标轴z轴的单位冲量带来的在x轴上的速度变化
				Vector3 velocityOnXPerUnitImpulse = deltaVelocityContact.GetRow(0);

				// 我们假设x轴上的冲量为单位冲量，也就是1，当前这种滑动摩擦情况下，y轴和z轴的冲量实际上是根据x轴的冲量计算出来的
				// 也就是yz平面上的冲量大小等于x轴上的u(滑动摩擦系数)倍，即也就是u，然后再把u分解到y轴和z轴上
				// 这个分解在上面通过除以planarImpulse已完成，结果在impulseContact.y/z中，所以此时y轴和z轴上的冲量就表示为mFriction * impulseContact.y/z
				// x轴上的速度变化会同时受xyz三个轴上的冲量影响，而y轴和z轴上的冲量大小又由x轴冲量和摩擦系数决定，所以最终x轴上的速度变化可以完全由x轴上的冲量决定
				// 假设x轴上为单位冲量，那么x轴上的单位冲量带来的x轴速度变化表示如下
				float velocityOnXPerUnitAxisXImpulse = velocityOnXPerUnitImpulse.x
					+ velocityOnXPerUnitImpulse.y * mFriction * impulseContact.y
					+ velocityOnXPerUnitImpulse.z * mFriction * impulseContact.z;

				// mDesiredDeltaVelocity即期望的在x轴上的速度变化，除以x轴上每单位冲量带来的速度变化量，就得到最终在x轴上需要的冲大小
				impulseContact.x = mDesiredDeltaVelocity / velocityOnXPerUnitAxisXImpulse;

				// u * x轴冲量 = yz平面冲量，再乘以yz平面上的单位冲量在y轴和z轴上的分量得到最终y轴和z轴的冲量
				impulseContact.y *= mFriction * impulseContact.x;
				impulseContact.z *= mFriction * impulseContact.x;
			}

			return impulseContact;
		}

		Vector3 Contact::CalculateFrictionlessImpulse(Matrix3* inverseInertiaTensor)
		{
			// 单位线性冲量带来的旋转导致的速度变化(解释见ResolvePenetration)
			Vector3 velocityPerUnitImpulseByRotation = Math::Cross(mRelativeContactPosition[0], mContactNormal);
			velocityPerUnitImpulseByRotation = inverseInertiaTensor[0] * velocityPerUnitImpulseByRotation;
			velocityPerUnitImpulseByRotation = Math::Cross(velocityPerUnitImpulseByRotation, mRelativeContactPosition[0]);

			// 由单位线性冲量造成的旋转所导致的碰撞点的速度变化量在碰撞法线上的分量(解释见ResolvePenetration)
			float velocityPerUnitImpulse = Math::Dot(velocityPerUnitImpulseByRotation, mContactNormal);

			// GetInverseMass获取的是由单位冲量造成的线性速度变化量(解释见ResolvePenetration)
			// 相加后得到由单位冲量造成的线性运动和角运动结合起来的速度变化量
			velocityPerUnitImpulse += mRigidBodies[0]->GetInverseMass();

			// 如果有第二个刚体，那么就把第二个刚体的速度变化量也加上
			if (mRigidBodies[1])
			{
				// 过程同上
				Vector3 v2 = Math::Cross(mRelativeContactPosition[1], mContactNormal);
				v2 = inverseInertiaTensor[1] * v2;
				v2 = Math::Cross(v2, mRelativeContactPosition[1]);

				velocityPerUnitImpulse += Math::Dot(v2, mContactNormal);

				velocityPerUnitImpulse += mRigidBodies[1]->GetInverseMass();
			}

			// 我们已经得到了单位冲量造成的速度变化量velocityPerUnitImpulse
			// 所以用期望的速度变化量除以velocityPerUnitImpulse就可以得到造成对应速度变化所需要的冲量
			// 碰撞产生的冲量方向是沿着碰撞法线的，这里返回的又是碰撞空间下的冲量，所以只计算x轴(规定碰撞法线在碰撞空间中是x轴方向)即可
			return Vector3(mDesiredDeltaVelocity / velocityPerUnitImpulse, 0.0f, 0.0f);
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
			Vector3 contactVelocity = Math::Transpose(mContactToWorld) * velocity;

			// 在不考虑反作用力的情况下，加速度带来的速度变化
			Vector3 deltaVelocity = rigidBody->GetLastAcceleration() * duration;
			// 转换到碰撞坐标系
			deltaVelocity = Math::Transpose(mContactToWorld) * deltaVelocity;
			// 去掉碰撞法线上的分量，只保留在碰撞平面上的速度
			deltaVelocity.x = 0.0f;
			// 添加碰撞平面上的速度变化(如果有摩擦力这个速度会在后面被抵消掉)
			contactVelocity += deltaVelocity;

			return contactVelocity;
		}
	}
}