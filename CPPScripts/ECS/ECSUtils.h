#pragma once
#include "../pubh.h"

namespace ZXEngine
{
	namespace ECS
	{
		using Entity = uint32_t;
		using ComponentTypeID_T = uint32_t;
		using ComponentContainer = unordered_map<ComponentTypeID_T, void*>;

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

		template<typename Type>
		class TypeIDAllocator
		{
		public:
			template<typename T>
			static uint32_t Get()
			{
				static uint32_t id = ID++;
				return id;
			}

		private:
			inline static uint32_t ID = 0;
		};
	}
}