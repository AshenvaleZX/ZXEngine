#include "CollisionData.h"
#include "Contact.h"

namespace ZXEngine
{
	namespace PhysZ
	{
		CollisionData::CollisionData(uint32_t maxContacts)
		{
			mMaxContactCount = maxContacts;
			mCurContactCount = 0;
			mContactArray    = new Contact[maxContacts];
			mContactsLeft    = maxContacts;
			mCurContact      = mContactArray;
		}

		CollisionData::~CollisionData()
		{
			delete[] mContactArray;
		}

		void CollisionData::Reset()
		{
			mCurContactCount = 0;
			mContactsLeft = mMaxContactCount;
			mCurContact = mContactArray;
		}

		bool CollisionData::IsFull() const
		{
			return mContactsLeft <= 0;
		}

		void CollisionData::AddContacts(uint32_t count)
		{
			mContactsLeft    -= count;
			mCurContactCount += count;
			mCurContact      += count;
		}
	}
}