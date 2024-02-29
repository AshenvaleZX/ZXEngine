#include "Joint.h"

namespace ZXEngine
{
	namespace PhysZ
	{
		vector<Joint*> Joint::allJoints;

		Joint::Joint()
		{
			allJoints.push_back(this);
		}

		Joint::Joint(RigidBody* body0, RigidBody* body1) :
			mBodys{ body0, body1 }
		{
			allJoints.push_back(this);
		}

		Joint::Joint(RigidBody* body0, const Vector3& anchor0, RigidBody* body1, const Vector3& anchor1) :
			mBodys{ body0, body1 },
			mAnchor{ anchor0, anchor1 }
		{
			allJoints.push_back(this);
		}

		Joint::~Joint()
		{
			auto it = std::find(allJoints.begin(), allJoints.end(), this);
			if (it != allJoints.end())
				allJoints.erase(it);
		}

		void Joint::SetBodys(RigidBody* body0, RigidBody* body1)
		{
			mBodys[0] = body0;
			mBodys[1] = body1;
		}

		void Joint::SetAnchor(const Vector3& anchor0, const Vector3& anchor1)
		{
			mAnchor[0] = anchor0;
			mAnchor[1] = anchor1;
		}

		void Joint::SetBodyAndAnchor(RigidBody* body0, const Vector3& anchor0, RigidBody* body1, const Vector3& anchor1)
		{
			mBodys[0] = body0;
			mBodys[1] = body1;
			mAnchor[0] = anchor0;
			mAnchor[1] = anchor1;
		}
	}
}