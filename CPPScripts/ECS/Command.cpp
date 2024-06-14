#include "Command.h"

namespace ZXEngine
{
	namespace ECS
	{
		Command& Command::Destroy(Entity entity)
		{
			auto iter = mWorld.mEntities.find(entity);

			if (iter != mWorld.mEntities.end())
			{
				for (auto& [id, component] : iter->second)
				{
					auto& componentData = mWorld.mComponents[id];
					componentData.mPool.Destroy(component);
					componentData.mEntitySet.Remove(entity);
				}

				mWorld.mEntities.erase(iter);
			}

			return *this;
		}
	}
}