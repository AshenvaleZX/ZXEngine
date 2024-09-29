#pragma once
#include <type_traits>
#include "Factory/NumericFactory.h"
#include "Factory/EnumFactory.h"
#include "Factory/ClassFactory.h"
#include "Factory/VoidFactory.h"

namespace ZXEngine
{
	namespace Reflection
	{
		namespace Dynamic
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

			private:
				DefaultFactory() = default;
				~DefaultFactory() = default;

				DefaultFactory(DefaultFactory&&) = delete;
				DefaultFactory& operator=(DefaultFactory&&) = delete;

				DefaultFactory(const DefaultFactory&) = delete;
				DefaultFactory& operator=(const DefaultFactory&) = delete;
			};

			template <typename T>
			class Factory
			{
			public:
				static auto& GetFactory()
				{
					using type = std::remove_cv_t<std::remove_reference_t<T>>;

					if constexpr (std::is_arithmetic_v<type>)
						return NumericFactory<type>::Get();
					else if constexpr (std::is_enum_v<type>)
						return EnumFactory<type>::Get();
					else if constexpr (std::is_class_v<type>)
						return ClassFactory<type>::Get();
					else if constexpr (std::is_void_v<type>)
						return VoidFactory<type>::Get();
					else
						return DefaultFactory<type>::Get();
				}
			};

			template <typename T>
			const TypeInfo* GetTypeInfo()
			{
				return &Factory<T>::GetFactory().GetTypeInfo();
			}
		}
	}
}