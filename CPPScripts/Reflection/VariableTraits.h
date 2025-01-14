#pragma once
#include <type_traits>

namespace ZXEngine
{
	namespace Reflection
	{
		// 获取变量类型信息
		template <typename T>
		struct VariableType
		{
			using Type = T;
		};

		// 获取成员变量类型信息
		template <typename Class, typename T>
		struct VariableType<T Class::*>
		{
			using Type = T;
		};

		// 自动匹配获取变量类型信息
		template <typename T>
		using VariableType_T = typename VariableType<T>::Type;

		// 以“尾置返回类型”的形式声明函数，通过这种方式剥离指针
		namespace Internal
		{
			template <typename T>
			auto VariablePointerToType(T*) -> T {};

			template <typename Class, typename T>
			auto VariablePointerToType(T Class::*) -> T {};
		}

		// 获取指针类型数据的类型信息
		template <auto T>
		using VariablePointerToType_T = decltype(Internal::VariablePointerToType(T));

		namespace Internal
		{
			template <typename T>
			struct BasicVariableTraits
			{
				using Type = VariableType_T<T>;
				static constexpr bool isMember = std::is_member_pointer_v<T>;
			};
		}

		template <typename T>
		struct VariableTraits;

		template <typename T>
		struct VariableTraits<T*> : Internal::BasicVariableTraits<T>
		{
			using Pointer = T*;
		};

		template <typename ClassT, typename T>
		struct VariableTraits<T ClassT::*> : Internal::BasicVariableTraits<T ClassT::*>
		{
			using Pointer = T ClassT::*;
			using Class = ClassT;
		};

		namespace Internal
		{
			template <auto T>
			struct VariablePointerTraits : VariableTraits<decltype(T)> {};
		}

		template <auto T>
		using VariablePointerTraits = Internal::VariablePointerTraits<T>;
	}
}