#pragma once
#include "../pubh.h"

namespace ZXEngine
{
	namespace ECS
	{
		using Entity = uint32_t;
		using ComponentTypeID_T = uint32_t;

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

		class ComponentTypeIDAllocator
		{
		public:
			template<typename T>
			static ComponentTypeID_T Get()
			{
				static ComponentTypeID_T id = ID++;
				return id;
			}

		private:
			inline static ComponentTypeID_T ID = 0;
		};
	}
}