#include "World.h"
#include "Command.h"
#include "Queryer.h"

namespace ZXEngine
{
	namespace ECS
	{
		World* World::mInstance = nullptr;

		void World::Create()
		{
			mInstance = new World();
		}

		World* World::GetInstance()
		{
			return mInstance;
		}

		World& World::GetReference()
		{
			return *mInstance;
		}

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
				system(*this, command, queryer, mEvent);
				commands.push_back(command);
			}

			mEvent.RemoveAllEvents();
			mEvent.AddAllEvents();

			for (auto& command : commands)
			{
				command.Execute();
			}
		}

		World& World::AddStartSystem(StartSystem system)
		{
			mStartSystems.push_back(system);
			return *this;
		}

		World& World::AddUpdateSystem(UpdateSystem system)
		{
			mUpdateSystems.push_back(system);
			return *this;
		}
	}
}