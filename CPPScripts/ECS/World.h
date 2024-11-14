#pragma once
#include "../pubh.h"
#include "../Common/SparseNaturalSet.h"
#include "ECSUtils.h"
#include "Event.h"

namespace ZXEngine
{
	namespace ECS
	{
		class Command;
		class Queryer;

		using StartSystem = void(*)(Command&);
		using UpdateSystem = void(*)(ECS::World&, Command&, Queryer&, Event&);

		class World final
		{
			friend class Command;
			friend class Queryer;
		public:
			static void Create();
			static World* GetInstance();
			static World& GetReference();

		private:
			static World* mInstance;

		public:
			World() = default;

			World(const World&) = delete;
			World& operator=(const World&) = delete;

			World(World&&) = default;
			World& operator=(World&&) = default;

			void Start();
			void Update();

			World& AddStartSystem(StartSystem system);
			World& AddUpdateSystem(UpdateSystem system);

			template<typename T>
			bool HasSingleton() const
			{
				auto singletonID = TypeIDAllocator<World::Singleton>::Get<T>();
				return mSingletons.find(singletonID) != mSingletons.end();
			}

			template<typename T>
			T& GetSingleton()
			{
				auto singletonID = TypeIDAllocator<World::Singleton>::Get<T>();
				auto it = mSingletons.find(singletonID);
				if (it == mSingletons.end())
				{
					assert(false);
				}
				return *static_cast<T*>(it->second.mInstance);
			}

		private:
			Event mEvent;
			vector<StartSystem> mStartSystems;
			vector<UpdateSystem> mUpdateSystems;

			struct ComponentPool
			{
				vector<void*> mInstances;
				vector<void*> mInstanceCaches;

				using CreateFunction = void*(*)(void);
				using DestroyFunction = void(*)(void*);

				CreateFunction CreateComponent = nullptr;
				DestroyFunction DestroyComponent = nullptr;

				ComponentPool();
				ComponentPool(CreateFunction create, DestroyFunction destroy);

				ComponentPool(const ComponentPool&) = delete;
				ComponentPool& operator=(const ComponentPool&) = delete;

				void* Create();
				void Destroy(void* instance);
			};

			struct ComponentData
			{
				// ʵ�ʴ��Component�����ݳ�
				ComponentPool mPool;
				// ӵ�д�����Component������Entity
				SparseNaturalSet<Entity, 32> mEntitySet;

				ComponentData() = default;
				ComponentData(ComponentPool::CreateFunction create, ComponentPool::DestroyFunction destroy) : mPool(create, destroy) {}

				ComponentData(const ComponentData&) = delete;
				ComponentData& operator=(const ComponentData&) = delete;
			};

			// ���е�Entity�͸���ӵ�е�Component
			unordered_map<Entity, ComponentContainer> mEntities;
			// ���е�Component���ݣ���ComponentTypeID����
			unordered_map<ComponentTypeID_T, ComponentData> mComponents;

			struct Singleton
			{
				void* mInstance = nullptr;

				using DestroyFunction = void(*)(void*);

				DestroyFunction Destroy = nullptr;

				Singleton() = default;
				Singleton(DestroyFunction destroy) : Destroy(destroy) {}

				~Singleton() { Destroy(mInstance); }
			};

			unordered_map<uint32_t, Singleton> mSingletons;
		};
	}
}