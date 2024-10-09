#pragma once
#include "../Common/TypeList.h"

namespace ZXEngine
{
    namespace Reflection
    {
        template <typename T>
        struct FunctionType;

        // �洢��ͨ��������
        template <typename Ret, typename... Args>
        struct FunctionType<Ret(*)(Args...)>
        {
            using Type = Ret(*)(Args...);
        };

        // �洢���Ա��������
        template <typename Ret, typename Class, typename... Args>
        struct FunctionType<Ret(Class::*)(Args...)>
        {
            using Type = Ret(Class::*)(Args...);
        };

        // �洢�ೣ����Ա��������
        template <typename Ret, typename Class, typename... Args>
        struct FunctionType<Ret(Class::*)(Args...) const>
        {
            using Type = Ret(Class::*)(Args...) const;
        };

        // ��ȡ��������
        template <typename T>
        using FunctionType_T = typename FunctionType<T>::Type;

        // �ԡ�β�÷������͡�����ʽ����������ͨ�����ַ�ʽ�ӷ���ֵ���ͻ�ȡ��������
        namespace Internal
        {
            template <typename Ret, typename... Args>
            auto FunctionPointerToType(Ret(*)(Args...))
                -> Ret(*)(Args...) {};

            template <typename Ret, typename Class, typename... Args>
            auto FunctionPointerToType(Ret(Class::*)(Args...))
                -> Ret(Class::*)(Args...) {};

            template <typename Ret, typename Class, typename... Args>
            auto FunctionPointerToType(Ret(Class::*)(Args...) const)
                -> Ret(Class::*)(Args...) const {};
        }

        // �Ӻ���ָ���ȡ����ָ������
        template <auto T>
        using FunctionPointerToType_T = decltype(Internal::FunctionPointerToType(T));

        // �Ӻ���ָ���ȡ��������
        template <auto T>
        using FunctionTypeFromPointer_T = FunctionType_T<decltype(Internal::FunctionPointerToType(T))>;

        namespace Internal
        {
            template <typename T>
            struct BasicFunctionTraits;

            template <typename Ret, typename... Argss>
            struct BasicFunctionTraits<Ret(Argss...)>
            {
                using Args = TypeList<Argss...>;
                using ReturnType = Ret;
            };
        }

        // �洢���������������Ϣ�Ͳ���������Ϣ
        template <typename Func>
        struct FunctionTraits;

        template <typename Ret, typename... Args>
        struct FunctionTraits<Ret(Args...)> : Internal::BasicFunctionTraits<Ret(Args...)>
        {
            using Type = Ret(*)(Args...);
            using ArgsWithClass = TypeList<Args...>;
            using Pointer = Ret(*)(Args...);
            static constexpr bool isConst = false;
            static constexpr bool isMember = false;
        };

        template <typename Ret, typename Class, typename... Args>
        struct FunctionTraits<Ret(Class::*)(Args...)> : Internal::BasicFunctionTraits<Ret(Args...)>
        {
            using Type = Ret(Class::*)(Args...);
            using ArgsWithClass = TypeList<Class*, Args...>;
            using Pointer = Ret(Class::*)(Args...);
            static constexpr bool isConst = false;
            static constexpr bool isMember = true;
        };

        template <typename Ret, typename Class, typename... Args>
        struct FunctionTraits<Ret(Class::*)(Args...) const> : Internal::BasicFunctionTraits<Ret(Args...)>
        {
            using Type = Ret(Class::*)(Args...) const;
            using ArgsWithClass = TypeList<Class*, Args...>;
            using Pointer = Ret(Class::*)(Args...) const;
            static constexpr bool isConst = true;
            static constexpr bool isMember = true;
        };

        namespace Internal
        {
            template <auto T>
            struct FunctionPointerTraits : FunctionTraits<FunctionTypeFromPointer_T<T>> {};
        }

        template <auto T>
        using FunctionPointerTraits = Internal::FunctionPointerTraits<T>;

        template <typename T>
        constexpr bool IsFunction_V = std::is_function_v<T> || std::is_member_function_pointer_v<T>;
    }
}