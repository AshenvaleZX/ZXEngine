#pragma once
#include "../pubh.h"

namespace ZXEngine
{
	namespace ECS
	{
		using Entity = uint32_t;
		using ComponentID_T = uint32_t;

		template<typename T, typename = std::enable_if<std::is_integral_v<T>>>
		class IDGenerator
		{
		public:
			static T Gen()
			{
				return ID++;
			}

		private:
			inline static T ID = {};
		};

		using EntityGenerator = IDGenerator<Entity>;

		class ComponentIDAllocator
		{
		public:
			template<typename T>
			ComponentID_T Get()
			{
				static ComponentID_T id = ID++;
				return id;
			}

		private:
			inline static ComponentID_T ID = 0;
		};
	}
}