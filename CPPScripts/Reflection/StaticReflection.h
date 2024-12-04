#pragma once
#include <string>
#include <functional>
#include "VariableTraits.h"
#include "FunctionTraits.h"

// Reference: https://github.com/VisualGMQ/mirrow

namespace ZXEngine
{
    namespace Reflection
    {
        namespace Static
        {
            namespace Internal
            {
                template <typename T, bool>
                struct BasicFieldTraits;

                template <typename T>
                struct BasicFieldTraits<T, true> : FunctionTraits<T>
                {
                    constexpr bool IsConstMember() const noexcept
                    {
                        return FunctionTraits<T>::isConst;
                    }

                    constexpr bool IsMember() const noexcept
                    {
                        return FunctionTraits<T>::isMember;
                    }

                    constexpr bool IsFunction() const noexcept
                    {
                        return true;
                    }

                    constexpr bool IsVariable() const noexcept
                    {
                        return false;
                    }
                };

                template <typename T>
                struct BasicFieldTraits<T, false> : VariableTraits<T>
                {
                    constexpr bool IsConstMember() const noexcept
                    {
                        return false;
                    }

                    constexpr bool IsMember() const noexcept
                    {
                        return VariableTraits<T>::isMember;
                    }

                    constexpr bool IsFunction() const noexcept
                    {
                        return false;
                    }

                    constexpr bool IsVariable() const noexcept
                    {
                        return true;
                    }
                };

                inline constexpr std::string_view TrimFieldName(std::string_view name)
                {
                    if (auto idx = name.find_last_of('&'); idx != std::string_view::npos)
                        name = name.substr(idx + 1, name.length());

                    if (auto idx = name.find_last_of(':'); idx != std::string_view::npos)
                        name = name.substr(idx + 1, name.length());

                    return name;
                }
            }

            template <typename T, typename... Attrs>
            struct FieldTraits : Internal::BasicFieldTraits<T, IsFunction_V<T>>
            {
                constexpr FieldTraits(T&& pointer, std::string_view name, Attrs&&... attrs) :
                    mPointer(std::forward<T>(pointer)),
                    mName(Internal::TrimFieldName(name)),
                    mAttrs(std::forward<Attrs>(attrs)...)
                {}

                constexpr bool IsConstMember() const noexcept
                {
                    return Base::IsConstMember();
                }

                constexpr bool IsMember() const noexcept
                {
                    return Base::IsMember();
                }

                constexpr std::string_view GetName() const noexcept
                {
                    return mName;
                }

                constexpr auto GetPointer() const noexcept
                {
                    return mPointer;
                }

                constexpr auto& GetAttrs() const noexcept
                {
                    return mAttrs;
                }

                template <typename... Args>
                constexpr decltype(auto) Invoke(Args&&... args)
                {
                    if constexpr (IsFunction_V<T>)
                    {
                        // 调用函数
                        return std::invoke(this->mPointer, std::forward<Args>(args)...);
                    }
                    else
                    {
                        // 获取变量的值
                        if constexpr (VariableTraits<T>::isMember)
                        {
                            // 成员变量通过Invoke的方式获取
                            return std::invoke(this->mPointer, std::forward<Args>(args)...);
                        }
                        else
                        {
                            // 普通变量直接解引用获取
                            return *(this->mPointer);
                        }
                    }
                }

            private:
                using Base = Internal::BasicFieldTraits<T, IsFunction_V<T>>;

                T mPointer;
                std::string_view mName;
                std::tuple<Attrs...> mAttrs;
            };

            template <typename... Argss>
            struct Constructor
            {
                using Args = TypeList<Argss...>;
            };

            template <typename T>
            struct BaseTypeInfo
            {
                using Type = T;
                static constexpr bool isFinal = std::is_final_v<T>;
            };

            template <typename T>
            struct TypeInfo;

#define StaticReflection(Type, ...)                             \
        namespace Reflection                                    \
        {                                                       \
            namespace Static                                    \
            {                                                   \
                template <>                                     \
                struct TypeInfo<Type> : BaseTypeInfo<Type>      \
                {                                               \
                    static constexpr std::string_view GetName() \
                    {                                           \
                        return #Type;                           \
                    }                                           \
                    __VA_ARGS__                                 \
                };                                              \
            }                                                   \
        }

#define Field(p, ...) FieldTraits { p, #p, ##__VA_ARGS__ }

#define Fields(...) inline static constexpr auto mFields = std::make_tuple(__VA_ARGS__);

#define Constructor(...) Constructor<__VA_ARGS__>

#define Constructors(...) using Constructors = TypeList<__VA_ARGS__>;

            namespace Internal
            {
                // 第二个模板参数默认参数std::void_t<>
                // std::false_type内部包含一个value = false;
                // 相当于HasFields没有匹配到特化版本时value为false
                template <typename TypeInfo, typename = std::void_t<>>
                struct HasFields final : std::false_type {};

                // 利用SFINAE机制，在TypeInfo有mFields字段时匹配到此特化版本
                // 如果TypeInfo没有mFields，decltype(TypeInfo::mFields)会编译失败，从而通过SFINAE机制避免匹配到此特化版本
                template <typename TypeInfo>
                struct HasFields<TypeInfo, std::void_t<decltype(TypeInfo::mFields)>>
                {
                    static constexpr bool value = !IsListEmpty_V<std::remove_cv_t<std::remove_const_t<decltype(TypeInfo::mFields)>>>;
                };
            }

            template <typename TypeInfo>
            constexpr bool HasFields_V = Internal::HasFields<TypeInfo>::value;

            template <typename T>
            class ReflectInfo final
            {
            public:
                using Type = TypeInfo<T>;

                constexpr std::string_view GetName() const noexcept
                {
                    return Type::GetName();
                }

                template <typename... Args>
                T Construct(Args&&... args)
                {
                    return T{ std::forward<Args>(args)... };
                }

                constexpr bool IsClass() const noexcept
                {
                    return std::is_class_v<T>;
                }

                constexpr bool HasFields() const noexcept
                {
                    return HasFields_V<Type>;
                }

                template <typename Function>
                void VisitAllFields(Function&& func)
                {
                    if constexpr (HasFields_V<Type>)
                    {
                        std::apply
                        (
                            [&func](auto&&... args)
                            {
                                (func(std::forward<decltype(args)>(args)), ...);
                            },
                            Type::mFields
                        );
                    }
                }

                template <typename Function>
                void TraverseMemberVariableAndDo(Function&& func)
                {
                    if constexpr (HasFields_V<Type>)
                    {
                        DoTraverseMemberVariableAndDo<0>(std::forward<Function>(func));
                    }
                }

                template <typename Function>
                void TraverseMemberFunctionAndDo(Function&& func)
                {
                    if constexpr (HasFields_V<Type>)
                    {
                        DoTraverseMemberFunctionAndDo<0>(std::forward<Function>(func));
                    }
                }

            private:
                template <size_t I, typename Function>
                void DoTraverseMemberVariableAndDo(Function&& func)
                {
                    auto fields = Type::mFields;
                    if constexpr (I < ListSize_V<std::remove_cv_t<std::remove_reference_t<decltype(fields)>>>)
                    {
                        auto field = std::get<I>(fields);
                        if constexpr (field.IsVariable() && field.IsMember())
                        {
                            func(std::get<I>(fields));
                        }
                        DoTraverseMemberVariableAndDo<I + 1>(std::forward<Function>(func));
                    }
                }

                template <size_t I, typename Function>
                void DoTraverseMemberFunctionAndDo(Function&& func)
                {
                    constexpr auto fields = Type::mFields;
                    if constexpr (I < ListSize_V<std::remove_cv_t<std::remove_reference_t<decltype(fields)>>>)
                    {
                        constexpr auto field = std::get<I>(fields);
                        if constexpr (field.IsFunction() && field.IsMember())
                        {
                            func(std::get<I>(fields));
                        }
                        DoTraverseMemberFunctionAndDo<I + 1>(std::forward<Function>(func));
                    }
                }
            };

            template <typename T>
            constexpr auto Reflect()
            {
                return ReflectInfo<std::remove_cv_t<std::remove_reference_t<T>>>{};
            }

            template <typename T>
            constexpr auto Reflect(const T& t)
            {
                return ReflectInfo<std::remove_cv_t<std::remove_reference_t<T>>>{};
            }
        }
    }
}