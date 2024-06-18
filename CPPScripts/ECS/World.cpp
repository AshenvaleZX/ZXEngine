#include "World.h"
#include "Command.h"
#include "Queryer.h"

namespace ZXEngine
{
	namespace ECS
	{
		World::ComponentPool::ComponentPool()
		{
			Debug::LogError("Invalid ECS component pool creation!");
		}

		World::ComponentPool::ComponentPool(CreateFunction create, DestroyFunction destroy) :
			CreateComponent(create),
			DestroyComponent(destroy)
		{}

		void* World::ComponentPool::Create()
		{
			if (mInstanceCaches.empty())
			{
				mInstances.push_back(CreateComponent());
			}
			else
			{
				mInstances.push_back(mInstanceCaches.back());
				mInstanceCaches.pop_back();
			}
			return mInstances.back();
		}

		void World::ComponentPool::Destroy(void* instance)
		{
			auto it = std::find(mInstances.begin(), mInstances.end(), instance);
			if (it != mInstances.end())
			{
				mInstanceCaches.push_back(*it);
				std::swap(*it, mInstances.back());
				mInstances.pop_back();
			}
		}

		void World::Start()
		{
			vector<Command> commands;

			for (auto& system : mStartSystems)
			{
				Command command{*this};
				system(command);
				commands.push_back(command);
			}

			for (auto& command : commands)
			{
				command.Execute();
			}
		}

		void World::Update()
		{
			vector<Command> commands;

			for (auto& system : mUpdateSystems)
			{
				Command command{*this};
				Queryer queryer{*this};
				system(command, queryer);
				commands.push_back(command);
			}

			for (auto& command : commands)
			{
				command.Execute();
			}
		}

		void World::AddStartSystem(StartSystem system)
		{
			mStartSystems.push_back(system);
		}

		void World::AddUpdateSystem(UpdateSystem system)
		{
			mUpdateSystems.push_back(system);
		}
	}
}