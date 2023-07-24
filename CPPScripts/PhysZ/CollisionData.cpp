#include "CollisionData.h"
#include "Contact.h"

namespace ZXEngine
{
	namespace PhysZ
	{
		void CollisionData::AddContacts(uint32_t count)
		{
			mContactsLeft  -= count;
			mContactsCount += count;
			mCurContact    += count;
		}
	}
}