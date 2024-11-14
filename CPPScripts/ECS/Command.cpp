#include "Command.h"

namespace ZXEngine
{
	namespace ECS
	{
		void Command::Execute()
		{
			for (auto entity : mEntitiesToDestroy)
			{
				DestroyEntity(entity);
			}
			mEntitiesToDestroy.clear();

			for (auto singleton : mSingletonsToDestroy)
			{
				DoDestroySingleton(singleton);
			}
			mSingletonsToDestroy.clear();

			RealDoSpawnEntityD();
		}

		Command& Command::DestroyEntity(Entity entity)
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

		Command& Command::DestroyEntityD(Entity entity)
		{
			mEntitiesToDestroy.push_back(entity);
			return *this;
		}

		void Command::RealDoSpawnEntityD()
		{
			for (auto& entitySpawner : mEntitiesToSpawn)
			{
				auto iter = mWorld.mEntities.emplace(entitySpawner.entity, ComponentContainer{});
				auto& componentContainer = iter.first->second;

				for (auto& componentSpawner : entitySpawner.componentSpawners)
				{
					componentContainer[componentSpawner.ID] = SpawnComponentForEntity(entitySpawner.entity, componentSpawner);
				}
			}
			mEntitiesToSpawn.clear();
		}

		void* Command::SpawnComponentForEntity(Entity entity, const ComponentSpawner& spawner)
		{
			if (auto iter = mWorld.mComponents.find(spawner.ID); iter == mWorld.mComponents.end())
			{
				mWorld.mComponents.emplace(std::piecewise_construct, std::forward_as_tuple(spawner.ID), std::forward_as_tuple(spawner.Create, spawner.Destroy));
			}

			auto& componentData = mWorld.mComponents[spawner.ID];
			void* newComponent = componentData.mPool.Create();

			spawner.Assign(newComponent);
			componentData.mEntitySet.Add(entity);

			return newComponent;
		}

		void Command::DoDestroySingleton(uint32_t id)
		{
			if (auto iter = mWorld.mSingletons.find(id); iter != mWorld.mSingletons.end())
			{
				mWorld.mSingletons.erase(iter);
			}
			else
			{
				Debug::LogError("Try to destroy a non-existing singleton.");
			}
		}
	}
}