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
			Command(World& world) : mWorld(world) {}

			template<typename... ComponentTypes>
			Command& Spawn(ComponentTypes&&... components)
			{
				Entity entity = EntityGenerator::Gen();
				DoSpawn(entity, std::forward<ComponentTypes>(components)...);
				return *this;
			}

			Command& Destroy(Entity entity);

		private:
			World& mWorld;

			template<typename T, typename... Components>
			void DoSpawn(Entity entity, T&& component, Components&&... components)
			{
				auto componentID = ComponentTypeIDAllocator::Get<T>();
				if (auto iter = mWorld.mComponents.find(componentID); iter == mWorld.mComponents.end())
				{
					mWorld.mComponents.emplace(componentID, World::ComponentData(
						[]()->void* { return new T(); },
						[](void* ptr)->void { delete (T*)ptr; }
					));
				}

				auto& componentData = mWorld.mComponents[componentID];
				void* newComponent = componentData.mPool.Create();

				(*(T*)newComponent) = std::forward<T>(component);
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