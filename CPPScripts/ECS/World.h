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
		public:
			using ComponentContainer = unordered_map<ComponentID_T, void*>;

		private:
			struct ComponentPool
			{
				vector<void*> mInstances;
				vector<void*> mInstanceCaches;

				using CreateFunction = void*(*)(void);
				using DestroyFunction = void(*)(void*);

				CreateFunction CreateComponent;
				DestroyFunction DestroyComponent;

				ComponentPool(CreateFunction create, DestroyFunction destroy);

				void* Create();
				void Destroy(void* instance);
			};

			struct ComponentData
			{
				ComponentPool mPool;
				SparseNaturalSet<Entity, 32> mEntitySet;
			};

			using ComponentMap = unordered_map<ComponentID_T, ComponentData>;

			ComponentMap mComponentMap;
			unordered_map<Entity, ComponentContainer> mEntities;
		};
	}
}