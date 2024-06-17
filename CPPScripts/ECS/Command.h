#pragma once
#include "../pubh.h"
#include "World.h"

namespace ZXEngine
{
	namespace ECS
	{
		class Command
		{
		public:
			Command() = delete;
			Command(World& world) : mWorld(world) {}

			template<typename... ComponentTypes>
			Command& Spawn(ComponentTypes&&... components)
			{
				Entity entity = EntityGenerator::Gen();
				DoSpawn(entity, std::forward<ComponentTypes>(components)...);
				return *this;
			}

			Command& Destroy(Entity entity);

			template<typename T>
			Command& SpawnSingleton(T&& singleton)
			{
				auto singletonID = TypeIDAllocator<World::Singleton>::Get<T>();
				
				if (auto iter = mWorld.mSingletons.find(singletonID); iter == mWorld.mSingletons.end())
				{
					auto newSingleton = mWorld.mSingletons.emplace(singletonID, World::Singleton(
						[](void* ptr)->void { delete (T*)ptr; }
					));
					newSingleton.first->second.mInstance = new T();
					*(static_cast<T*>(newSingleton.first->second.mInstance)) = std::forward<T>(singleton);
				}
				else
				{
					Debug::LogError("Try to spawn an existing singleton.");
				}

				return *this;
			}

			template<typename T>
			Command& DestroySingleton()
			{
				auto singletonID = TypeIDAllocator<World::Singleton>::Get<T>();

				if (auto iter = mWorld.mSingletons.find(singletonID); iter != mWorld.mSingletons.end())
				{
					mWorld.mSingletons.erase(iter);
				}
				else
				{
					Debug::LogError("Try to destroy a non-existing singleton.");
				}

				return *this;
			}

		private:
			World& mWorld;

			template<typename T, typename... Components>
			void DoSpawn(Entity entity, T&& component, Components&&... components)
			{
				auto componentID = TypeIDAllocator<World::ComponentPool>::Get<T>();

				if (auto iter = mWorld.mComponents.find(componentID); iter == mWorld.mComponents.end())
				{
					mWorld.mComponents.emplace(componentID, World::ComponentData(
						[]()->void* { return new T(); },
						[](void* ptr)->void { delete (T*)ptr; }
					));
				}

				auto& componentData = mWorld.mComponents[componentID];
				void* newComponent = componentData.mPool.Create();

				*(static_cast<T*>(newComponent)) = std::forward<T>(component);
				componentData.mEntitySet.Add(entity);

				auto iter = mWorld.mEntities.emplace(entity);
				iter.first->second[componentID] = newComponent;

				if constexpr (sizeof...(components) > 0)
				{
					DoSpawn<Components...>(entity, std::forward<Components>(components)...);
				}
			}
		};
	}
}