#include "World.h"

namespace ZXEngine
{
	namespace ECS
	{
		World::ComponentPool::ComponentPool(CreateFunction create, DestroyFunction destroy) :
			CreateComponent(create),
			DestroyComponent(destroy)
		{}

		void* World::ComponentPool::Create()
		{
			if (mInstanceCaches.empty())
			{
				mInstances.push_back(CreateComponent());
			}
			else
			{
				mInstances.push_back(mInstanceCaches.back());
				mInstanceCaches.pop_back();
			}
			return mInstances.back();
		}

		void World::ComponentPool::Destroy(void* instance)
		{
			auto it = std::find(mInstances.begin(), mInstances.end(), instance);
			if (it != mInstances.end())
			{
				mInstanceCaches.push_back(*it);
				std::swap(*it, mInstances.back());
				mInstances.pop_back();
			}
		}
	}
}