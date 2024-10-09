#pragma once

namespace ZXEngine
{
    template <typename... Ts>
    struct TypeList
    {
        using SelfType = TypeList<Ts...>;
        static constexpr size_t size = sizeof...(Ts);
    };

    namespace Internal
    {
        // ��������δʵ��ʹ��ģ�壬ʡ����T���ȼ���template <typename T>
        template <typename>
        struct ListSize;

        // template <typename...> �� ListType �Ķ�����ȷ���� ListType ��һ�����ܿɱ��������Ͳ�����ģ��
        // ���û�����Լ����ListType ����ֻ��һ����ͨ�����ͣ�������һ��ģ��
        template <template <typename...> typename ListType, typename... Ts>
        struct ListSize<ListType<Ts...>>
        {
            static constexpr size_t value = sizeof...(Ts);
        };
    }

    // TypeList ת std::tuple
    template <typename TypeList>
    struct TypeListToTuple;

    template <typename... Ts>
    struct TypeListToTuple<TypeList<Ts...>>
    {
        using type = std::tuple<Ts...>;
    };

    template <typename List>
    constexpr size_t ListSize_V = Internal::ListSize<List>::value;

    template <typename List>
    constexpr bool IsListEmpty_V = ListSize_V<List> == 0;
}