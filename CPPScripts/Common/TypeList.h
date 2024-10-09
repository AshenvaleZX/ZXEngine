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
        // 仅声明，未实际使用模板，省略了T，等价于template <typename T>
        template <typename>
        struct ListSize;

        // template <typename...> 在 ListType 的定义中确保了 ListType 是一个接受可变数量类型参数的模板
        // 如果没有这个约束，ListType 可能只是一个普通的类型，而不是一个模板
        template <template <typename...> typename ListType, typename... Ts>
        struct ListSize<ListType<Ts...>>
        {
            static constexpr size_t value = sizeof...(Ts);
        };
    }

    // TypeList 转 std::tuple
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