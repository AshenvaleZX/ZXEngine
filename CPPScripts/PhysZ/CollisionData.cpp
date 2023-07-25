#include "CollisionData.h"
#include "Contact.h"

namespace ZXEngine
{
	namespace PhysZ
	{
		bool CollisionData::IsFull() const
		{
			return mContactsLeft <= 0;
		}

		void CollisionData::AddContacts(uint32_t count)
		{
			mContactsLeft  -= count;
			mContactsCount += count;
			mCurContact    += count;
		}
	}
}