#include "ContactResolver.h"
#include "Contact.h"

namespace ZXEngine
{
	namespace PhysZ
	{
		ContactResolver::ContactResolver(uint32_t maxIterations, float velocityEpsilon, float positionEpsilon) :
			mMaxVelocityIterations(maxIterations),
			mMaxPositionIterations(maxIterations),
			mVelocityEpsilon(velocityEpsilon), 
			mPositionEpsilon(positionEpsilon)
		{}

		ContactResolver::ContactResolver(uint32_t maxVelocityIterations, uint32_t maxPositionIterations, float velocityEpsilon, float positionEpsilon) :
			mMaxVelocityIterations(maxVelocityIterations),
			mMaxPositionIterations(maxPositionIterations),
			mVelocityEpsilon(velocityEpsilon), 
			mPositionEpsilon(positionEpsilon)
		{}

		void ContactResolver::SetMaxIterations(uint32_t maxIterations)
		{
			mMaxVelocityIterations = maxIterations;
			mMaxPositionIterations = maxIterations;
		}

		void ContactResolver::SetVelocityEpsilon(float velocityEpsilon)
		{
			mVelocityEpsilon = velocityEpsilon;
		}

		void ContactResolver::SetPositionEpsilon(float positionEpsilon)
		{
			mPositionEpsilon = positionEpsilon;
		}
	}
}