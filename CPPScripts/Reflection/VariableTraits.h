#pragma once
#include <type_traits>

namespace ZXEngine
{
	namespace Reflection
	{
		// ��ȡ����������Ϣ
		template <typename T>
		struct VariableType
		{
			using Type = T;
		};

		// ��ȡ��Ա����������Ϣ
		template <typename Class, typename T>
		struct VariableType<T Class::*>
		{
			using Type = T;
		};

		// �Զ�ƥ���ȡ����������Ϣ
		template <typename T>
		using VariableType_T = typename VariableType<T>::Type;

		// �ԡ�β�÷������͡�����ʽ����������ͨ�����ַ�ʽ����ָ��
		namespace Internal
		{
			template <typename T>
			auto VariablePointerToType(T*) -> T {};

			template <typename Class, typename T>
			auto VariablePointerToType(T Class::*) -> T {};
		}

		// ��ȡָ���������ݵ�������Ϣ
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