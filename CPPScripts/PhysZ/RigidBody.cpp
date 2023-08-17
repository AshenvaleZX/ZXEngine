#include "RigidBody.h"
#include "PhysZEnumStruct.h"
#include "CollisionPrimitive.h"

namespace ZXEngine
{
	namespace PhysZ
	{
		RigidBody::~RigidBody()
		{
			if (mCollisionVolume->mRigidBody == this)
			{
				mCollisionVolume->mRigidBody = nullptr;
			}
		}

		void RigidBody::Integrate(float duration)
		{
			if (!mIsAwake)
				return;

			// 通过合力计算加速度
			mLastAcceleration = mAcceleration;
			mLastAcceleration += mForceAccum * mInverseMass;

			// 通过合力矩计算角加速度
			Vector3 angularAcceleration = mWorldInverseInertiaTensor * mTorqueAccum;

			// 根据加速度更新速度
			mVelocity += mLastAcceleration * duration;

			// 根据角加速度更新角速度
			mAngularVelocity += angularAcceleration * duration;

			// 施加阻尼效果
			mVelocity *= pow(mLinearDamping, duration);
			mAngularVelocity *= pow(mAngularDamping, duration);

			// 根据速度移动位置
			mPosition += mVelocity * duration;

			// 根据角速度更新旋转状态
			mRotation.RotateByVector(mAngularVelocity, duration);

			// 更新其它数据
			CalculateDerivedData();

			// 清除累积的作用力和力矩
			ClearAccumulators();

			if (mCanSleep)
			{
				// 计算当前的运动值
				float currentMotion = Math::Dot(mVelocity, mVelocity) + Math::Dot(mAngularVelocity, mAngularVelocity);

				// 与之前的帧的运动值数据做累计平均
				float bias = powf(0.5f, duration);
				mMotion = bias * mMotion + (1 - bias) * currentMotion;

				// 如果累计平均运动值太小了就当作静态对象，进入休眠状态，节约不必要的物理计算
				if (mMotion < SleepMotionEpsilon)
					SetAwake(false);
				// 对累计运动值做一个最大值限制，如果一个高速移动物体突然停下来，会导致mMotion很大，然后需要很多帧才能把数据平均值拉下来
				// 导致一个实际已经静止的物体要等很久才能进入休眠状态，所以这里如果值太大了进行限制
				else if (mMotion > 10 * SleepMotionEpsilon)
					mMotion = 10 * SleepMotionEpsilon;
			}
		}

		void RigidBody::CalculateDerivedData()
		{
			UpdateTransform();
			UpdateWorldInertiaTensor();
		}

		void RigidBody::UpdateTransform()
		{
			mTransform = mRotation.ToMatrix();
			mTransform = Math::Translate(mTransform, mPosition);

			// 如果有碰撞体，同步碰撞体的Transform
			if (mCollisionVolume)
				mCollisionVolume->mTransform = mTransform;
		}

		void RigidBody::UpdateWorldInertiaTensor()
		{
			// 根据当前的transform信息，重新计算世界坐标系下的惯性张量
			Matrix3 rot(mTransform);
			// 惯性张量的变换公式为: I' = R * I * R^T，只需要考虑旋转变换即可
			mWorldInverseInertiaTensor = rot * mLocalInverseInertiaTensor * Math::Transpose(rot);
		}

		void RigidBody::AddForce(const Vector3& force)
		{
			mForceAccum += force;
			mIsAwake = true;
		}

		void RigidBody::AddForceAtPoint(const Vector3& force, const Vector3& point)
		{
			// 相对于质心的位置
			auto pos = point - mPosition;

			mForceAccum += force;
			mTorqueAccum += Math::Cross(pos, force);

			mIsAwake = true;
		}

		void RigidBody::AddForceAtLocalPoint(const Vector3& force, const Vector3& point)
		{
			AddForceAtPoint(force, mTransform * Vector4(point, 1.0f));
		}

		void RigidBody::AddTorque(const Vector3& torque)
		{
			mTorqueAccum += torque;
			mIsAwake = true;
		}

		void RigidBody::ClearAccumulators()
		{
			mForceAccum.Clear();
			mTorqueAccum.Clear();
		}

		bool RigidBody::IsInfiniteMass() const
		{
			return mInverseMass <= FLT_EPSILON;
		}

		void RigidBody::SetAwake(bool awake)
		{
			if (awake)
			{
				mIsAwake = true;

				// 唤醒后添加一点运动量，防止又立刻进入睡眠状态
				mMotion = SleepMotionEpsilon * 2.0f;
			}
			else
			{
				mIsAwake = false;

				// 休眠状态的刚体不应该有任何速度
				mVelocity.Clear();
				mAngularVelocity.Clear();
			}
		}

		bool RigidBody::GetAwake() const
		{
			return mIsAwake;
		}
		
		void RigidBody::SetCanSleep(bool canSleep)
		{
			mCanSleep = canSleep;

			if (!canSleep && !mIsAwake)
				SetAwake(true);
		}

		bool RigidBody::GetCanSleep() const
		{
			return mCanSleep;
		}

		Matrix4 RigidBody::GetTransform() const
		{
			return mTransform;
		}

		void RigidBody::SetMass(float mass)
		{
			if (mass <= 0.0f)
			{
				// 质量必须大于0，否则输出异常并将质量设置为无穷大
				Debug::LogError("Mass must be greater than zero.");
				mInverseMass = 0.0f;
			}
			else
			{
				mInverseMass = 1.0f / mass;
			}
		}

		float RigidBody::GetMass() const
		{
			return 1.0f / mInverseMass;
		}

		void RigidBody::SetInverseMass(float inverseMass)
		{
			mInverseMass = inverseMass;
		}

		float RigidBody::GetInverseMass() const
		{
			return mInverseMass;
		}

		void RigidBody::SetLinearDamping(float damping)
		{
			mLinearDamping = damping;
		}

		float RigidBody::GetLinearDamping() const
		{
			return mLinearDamping;
		}

		void RigidBody::SetPosition(const Vector3& position)
		{
			mPosition = position;
		}

		void RigidBody::GetPosition(Vector3& position) const
		{
			position = mPosition;
		}

		Vector3 RigidBody::GetPosition() const
		{
			return mPosition;
		}

		void RigidBody::SetVelocity(const Vector3& velocity)
		{
			mVelocity = velocity;
		}

		void RigidBody::GetVelocity(Vector3& velocity) const
		{
			velocity = mVelocity;
		}

		Vector3 RigidBody::GetVelocity() const
		{
			return mVelocity;
		}

		void RigidBody::AddVelocity(const Vector3& deltaVelocity)
		{
			mVelocity += deltaVelocity;
		}

		void RigidBody::SetAcceleration(const Vector3& acceleration)
		{
			mAcceleration = acceleration;
		}

		void RigidBody::GetAcceleration(Vector3& acceleration) const
		{
			acceleration = mAcceleration;
		}

		Vector3 RigidBody::GetAcceleration() const
		{
			return mAcceleration;
		}

		void RigidBody::GetLastAcceleration(Vector3& acceleration) const
		{
			acceleration = mLastAcceleration;
		}

		Vector3 RigidBody::GetLastAcceleration() const
		{
			return mLastAcceleration;
		}

		void RigidBody::SetInertiaTensor(const Matrix3& inertiaTensor)
		{
			mLocalInverseInertiaTensor = Math::Inverse(inertiaTensor);
		}

		void RigidBody::GetInertiaTensor(Matrix3& inertiaTensor) const
		{
			inertiaTensor = Math::Inverse(mLocalInverseInertiaTensor);
		}

		Matrix3 RigidBody::GetInertiaTensor() const
		{
			return Math::Inverse(mLocalInverseInertiaTensor);
		}

		void RigidBody::SetInverseInertiaTensor(const Matrix3& inverseInertiaTensor)
		{
			mLocalInverseInertiaTensor = inverseInertiaTensor;
		}

		void RigidBody::GetInverseInertiaTensor(Matrix3& inverseInertiaTensor) const
		{
			inverseInertiaTensor = mLocalInverseInertiaTensor;
		}

		Matrix3 RigidBody::GetInverseInertiaTensor() const
		{
			return mLocalInverseInertiaTensor;
		}

		void RigidBody::GetInverseInertiaTensorWorld(Matrix3& inverseInertiaTensor) const
		{
			inverseInertiaTensor = mWorldInverseInertiaTensor;
		}

		Matrix3 RigidBody::GetInverseInertiaTensorWorld() const
		{
			return mWorldInverseInertiaTensor;
		}

		void RigidBody::SetAngularDamping(float damping)
		{
			mAngularDamping = damping;
		}

		float RigidBody::GetAngularDamping() const
		{
			return mAngularDamping;
		}

		void RigidBody::SetRotation(const Quaternion& rotation)
		{
			mRotation = rotation;
		}

		void RigidBody::GetRotation(Quaternion& rotation) const
		{
			rotation = mRotation;
		}

		Quaternion RigidBody::GetRotation() const
		{
			return mRotation;
		}

		void RigidBody::SetAngularVelocity(const Vector3& angularVelocity)
		{
			mAngularVelocity = angularVelocity;
		}

		void RigidBody::GetAngularVelocity(Vector3& angularVelocity) const
		{
			angularVelocity = mAngularVelocity;
		}

		Vector3 RigidBody::GetAngularVelocity() const
		{
			return mAngularVelocity;
		}

		void RigidBody::AddAngularVelocity(const Vector3& deltaAngularVelocity)
		{
			mAngularVelocity += deltaAngularVelocity;
		}
	}
}