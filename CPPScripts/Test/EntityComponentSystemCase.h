#pragma once
#include "../pubh.h"
#include "../ECS/ECS.h"

namespace ZXEngine
{
	namespace Test
	{
		struct Name
		{
			string name;
		};

		struct Age
		{
			int age;
		};

		struct Timer
		{
			int time;
		};

		void TestStartSystem(ECS::Command& command)
		{
			command.SpawnSingleton<Timer>(Timer{ 0 })
				.SpawnEntity<Name>(Name{ "Tom" })
				.SpawnEntity<Age>(Age{ 20 })
				.SpawnEntity<Name, Age>(Name{ "Jerry" }, Age{ 30 });
		}

		void QueryAndLogSystem(ECS::World& world, ECS::Command& command, ECS::Queryer& queryer, ECS::Event& event)
		{
			for (auto entity : queryer.Query<Name>())
			{
				auto& name = queryer.Get<Name>(entity);
				Debug::Log("Name: %s", name.name);
			}

			for (auto entity : queryer.Query<Age>())
			{
				auto& age = queryer.Get<Age>(entity);
				Debug::Log("Age: %s", age.age);
			}

			for (auto entity : queryer.Query<Name, Age>())
			{
				auto& name = queryer.Get<Name>(entity);
				auto& age = queryer.Get<Age>(entity);
				Debug::Log("Name: %s, Age: %s", name.name, age.age);
			}

			static int time = 0;
			event.Writer<int>().Write(++time);
		}

		void EchoTimeSystem(ECS::World& world, ECS::Command& command, ECS::Queryer& queryer, ECS::Event& event)
		{
			auto& timer = world.GetSingleton<Timer>();

			auto reader = event.Reader<int>();
			if (reader.Has())
			{
				timer.time = reader.Read();
			}

			Debug::Log("------ Time: %s ------", timer.time);
		}

		void RunEntityComponentSystemCase()
		{
			ECS::World world;
			ECS::Command command(world);
			ECS::Queryer queryer(world);

			world.AddStartSystem(TestStartSystem)
				.AddUpdateSystem(QueryAndLogSystem)
				.AddUpdateSystem(EchoTimeSystem);

			world.Start();

			command.SpawnEntityD<Name>(Name{ "Jack" })
				.SpawnEntityD<Age>(Age{ 28 })
				.SpawnEntityD<Name, Age>(Name{ "Rose" }, Age{ 26 });

			world.Update();

			for (auto entity : queryer.Query<Age>())
			{
				command.DestroyEntity(entity);
			}

			world.Update();

			command.Execute();

			for (auto entity : queryer.Query<Name>())
			{
				command.DestroyEntityD(entity);
			}

			world.Update();

			command.Execute();

			world.Update();

			command.DestroySingleton<Timer>();
		}
	}
}