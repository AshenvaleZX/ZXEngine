#include "FGSpring.h"
#include "../RigidBody.h"

namespace ZXEngine
{
	namespace PhysZ
	{
		FGSpring::FGSpring(const Vector3& connectionPoint, const Vector3& otherConnectionPoint, RigidBody* other, float springConstant, float restLength) :
			mConnectionPoint(connectionPoint, 1.0f), 
			mOtherConnectionPoint(otherConnectionPoint, 1.0f),
			mOther(other), 
			mSpringConstant(springConstant), 
			mRestLength(restLength)
		{
            mType = ForceGeneratorType::Spring;
        }

        void FGSpring::UpdateForce(RigidBody* rigidBody, float duration)
        {
            // 弹簧链接的两端在世界坐标系下的位置
            Vector3 lws = rigidBody->GetTransform() * mConnectionPoint;
            Vector3 ows = mOther->GetTransform() * mOtherConnectionPoint;
            // 间距
            Vector3 dis = lws - ows;
            // 弹簧当前长度
            float length = dis.GetMagnitude();
            // 弹簧伸缩长度
            float delta = length - mRestLength;
            // 弹力大小
            float forceScalar = delta * mSpringConstant;
            // 弹力向量
            Vector3 force = -forceScalar * dis.GetNormalized();
            // 施加到当前刚体上
            rigidBody->AddForceAtPoint(force, lws);
        }
	}
}