#pragma once
#include <type_traits>
#include "Factory/NumericFactory.h"
#include "Factory/EnumFactory.h"
#include "Factory/ClassFactory.h"

namespace ZXEngine
{
	namespace Reflection
	{
		template <typename T>
		class DefaultFactory
		{
		public:
			static auto& Get()
			{
				static DefaultFactory instance;
				return instance;
			}
		};

		template <typename T>
		class Factory
		{
		public:
			static auto& GetFactory()
			{
				using type = std::remove_cv_t<std::remove_reference_t<T>>;

				if constexpr (std::is_fundamental_v<type>)
					return NumericFactory<type>::Get();
				else if constexpr (std::is_enum_v<type>)
					return EnumFactory<type>::Get();
				else if constexpr (std::is_class_v<type>)
					return ClassFactory<type>::Get();
				else
					return DefaultFactory<type>::Get();
			}
		};

		template <typename T>
		const BaseType* GetTypeInfo()
		{
			return &Factory<T>::GetFactory().GetTypeInfo();
		}
	}
}