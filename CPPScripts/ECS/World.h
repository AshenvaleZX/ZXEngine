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
		};
	}
}