#pragma once
#include "../pubh.h"
#include "../Common/SparseNaturalSet.h"
#include "ECSUtils.h"

namespace ZXEngine
{
	namespace ECS
	{
		class Command;
		class Queryer;

		using StartSystem = void(*)(Command);
		using UpdateSystem = void(*)(Command, Queryer);

		class World final
		{
			friend class Command;
			friend class Queryer;
		public:
			World() = default;
			World(const World&) = delete;
			World& operator=(const World&) = delete;

			void Start();
			void Update();

			void AddStartSystem(StartSystem system);
			void AddUpdateSystem(UpdateSystem system);

		private:
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