#pragma once
#include "../pubh.h"
#include "../Common/SparseNaturalSet.h"
#include "ECSUtils.h"

namespace ZXEngine
{
	namespace ECS
	{
		class World final
		{
			friend class Command;
		public:
			using ComponentContainer = unordered_map<ComponentTypeID_T, void*>;

		private:
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
				// 实际存放Component的数据池
				ComponentPool mPool;
				// 拥有此类型Component的所有Entity
				SparseNaturalSet<Entity, 32> mEntitySet;

				ComponentData() = default;
				ComponentData(ComponentPool::CreateFunction create, ComponentPool::DestroyFunction destroy) : mPool(create, destroy) {}
			};

			// 所有的Entity和各自拥有的Component
			unordered_map<Entity, ComponentContainer> mEntities;
			// 所有的Component数据，按ComponentTypeID归类
			unordered_map<ComponentTypeID_T, ComponentData> mComponents;
		};
	}
}