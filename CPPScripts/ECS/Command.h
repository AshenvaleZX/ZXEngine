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
			Command() : mWorld(World::GetReference()) {}
			Command(World& world) : mWorld(world) {}

			void Execute();

			template<typename... ComponentTypes>
			Command& SpawnEntity(ComponentTypes&&... components);

			template<typename... ComponentTypes>
			Command& SpawnEntityD(ComponentTypes&&... components);

			Command& DestroyEntity(Entity entity);
			Command& DestroyEntityD(Entity entity);

			template<typename T>
			Command& SpawnSingleton(T&& singleton);

			template<typename T>
			Command& DestroySingleton();
			
			template<typename T>
			Command& DestroySingletonD();

		private:
			World& mWorld;

			vector<Entity> mEntitiesToDestroy;
			vector<uint32_t> mSingletonsToDestroy;

			using AssignFunction = std::function<void(void*)>;

			struct ComponentSpawner
			{
				ComponentTypeID_T ID = 0;
				AssignFunction Assign = nullptr;
				World::ComponentPool::CreateFunction Create = nullptr;
				World::ComponentPool::DestroyFunction Destroy = nullptr;
			};

			struct EntitySpawner
			{
				Entity entity;
				vector<ComponentSpawner> componentSpawners;
			};

			vector<EntitySpawner> mEntitiesToSpawn;

			template<typename T, typename... Components>
			void DoSpawnEntity(Entity entity, T&& component, Components&&... components);

			template<typename T, typename... Components>
			void DoSpawnEntityD(Entity entity, vector<ComponentSpawner>& spawners, T&& component, Components&&... components);

			void RealDoSpawnEntityD();
			void* SpawnComponentForEntity(Entity entity, const ComponentSpawner& spawner);

			void DoDestroySingleton(uint32_t id);
		};

		template<typename... ComponentTypes>
		Command& Command::SpawnEntity(ComponentTypes&&... components)
		{
			Entity entity = EntityGenerator::Gen();
			DoSpawnEntity(entity, std::forward<ComponentTypes>(components)...);
			return *this;
		}

		template<typename... ComponentTypes>
		Command& Command::SpawnEntityD(ComponentTypes&&... components)
		{
			EntitySpawner entitySpawner;
			entitySpawner.entity = EntityGenerator::Gen();
			DoSpawnEntityD(entitySpawner.entity, entitySpawner.componentSpawners, std::forward<ComponentTypes>(components)...);
			mEntitiesToSpawn.push_back(std::move(entitySpawner));
			return *this;
		}

		template<typename T>
		Command& Command::SpawnSingleton(T&& singleton)
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
		Command& Command::DestroySingleton()
		{
			auto singletonID = TypeIDAllocator<World::Singleton>::Get<T>();

			DoDestroySingleton(singletonID);

			return *this;
		}

		template<typename T>
		Command& Command::DestroySingletonD()
		{
			auto singletonID = TypeIDAllocator<World::Singleton>::Get<T>();

			mSingletonsToDestroy.push_back(singletonID);

			return *this;
		}

		template<typename T, typename... Components>
		void Command::DoSpawnEntity(Entity entity, T&& component, Components&&... components)
		{
			auto componentID = TypeIDAllocator<World::ComponentPool>::Get<T>();

			if (auto iter = mWorld.mComponents.find(componentID); iter == mWorld.mComponents.end())
			{
				mWorld.mComponents.emplace(std::piecewise_construct, std::forward_as_tuple(componentID), std::forward_as_tuple(
					[]()->void* { return new T(); },
					[](void* ptr)->void { delete (T*)ptr; }
				));
			}

			auto& componentData = mWorld.mComponents[componentID];
			void* newComponent = componentData.mPool.Create();

			*(static_cast<T*>(newComponent)) = std::forward<T>(component);
			componentData.mEntitySet.Add(entity);

			auto iter = mWorld.mEntities.emplace(entity, ComponentContainer{});
			iter.first->second[componentID] = newComponent;

			if constexpr (sizeof...(components) > 0)
			{
				DoSpawnEntity<Components...>(entity, std::forward<Components>(components)...);
			}
		}

		template<typename T, typename... Components>
		void Command::DoSpawnEntityD(Entity entity, vector<ComponentSpawner>& spawners, T&& component, Components&&... components)
		{
			ComponentSpawner spawner;
			spawner.ID = TypeIDAllocator<World::ComponentPool>::Get<T>();

			// 这里如果component是以左值传入的，会产生复制开销，所以在调用时尽可能的使用右值传入
			spawner.Assign = [capturedComponent = std::forward<T>(component)](void* ptr)mutable->void
			{
				*(static_cast<T*>(ptr)) = std::move(capturedComponent);
			};

			spawner.Create = [](void)->void*
			{
				return new T();
			};

			spawner.Destroy = [](void* ptr)->void
			{
				delete static_cast<T*>(ptr);
			};

			spawners.push_back(spawner);

			if constexpr (sizeof...(components) > 0)
			{
				DoSpawnEntityD<Components...>(entity, spawners, std::forward<Components>(components)...);
			}
		}
	}
}