#pragma once
#include "../pubh.h"
#include "World.h"

namespace ZXEngine
{
	namespace ECS
	{
		class Queryer
		{
		public:
			Queryer() = delete;
			Queryer(World& world) : mWorld(world) {}

			template<typename... Components>
			vector<Entity> Query()
			{
				vector<Entity> res;
				DoQuery<Components...>(res);
				return res;
			}

			template<typename T>
			bool Has(Entity entity)
			{
				auto iter = mWorld.mEntities.find(entity);
				auto id = TypeIDAllocator<World::ComponentPool>::Get<T>();
				return iter != mWorld.mEntities.end() && iter->second.find(id) != iter->second.end();
			}

			template<typename T>
			T& Get(Entity entity)
			{
				auto id = TypeIDAllocator<World::ComponentPool>::Get<T>();
				return *(static_cast<T*>(mWorld.mEntities[entity][id]));
			}

			template<typename T>
			bool HasSingleton()
			{
				auto id = TypeIDAllocator<World::Singleton>::Get<T>();
				return mWorld.mSingletons.find(id) != mWorld.mSingletons.end();
			}

			template<typename T>
			T& GetSingleton()
			{
				auto id = TypeIDAllocator<World::Singleton>::Get<T>();
				return *(static_cast<T*>(mWorld.mSingletons[id].mInstance));
			}

		private:
			World& mWorld;

			template<typename T, typename... Components>
			void DoQuery(vector<Entity>& entities)
			{
				auto id = TypeIDAllocator<World::ComponentPool>::Get<T>();
				auto& data = mWorld.mComponents[id];

				if constexpr (sizeof...(Components) == 0)
				{
					data.mEntitySet.CopyTo(entities);
					return;
				}
				else
				{
					for (auto entity : data.mEntitySet)
					{
						const ComponentContainer& componentContainer = mWorld.mEntities[entity];
						if (CheckRemainComponents<Components...>(entity, componentContainer))
						{
							entities.push_back(entity);
						}
					}
				}
			}

			template<typename T, typename... Components>
			bool CheckRemainComponents(Entity entity, const ComponentContainer& componentContainer)
			{
				auto id = TypeIDAllocator<World::ComponentPool>::Get<T>();

				if (componentContainer.find(id) == componentContainer.end())
				{
					return false;
				}

				if constexpr (sizeof...(Components) == 0)
				{
					return true;
				}
				else
				{
					return CheckRemainComponents<Components...>(entity, componentContainer);
				}
			}
		};
	}
}