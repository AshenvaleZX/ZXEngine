#pragma once
#include "../../pubh.h"
#include "BaseType.h"
#include "../VariableTraits.h"
#include "../FunctionTraits.h"

namespace ZXEngine
{
	namespace Reflection
	{
		struct MemberVariable
		{
		public:
			string name;
			const BaseType* type;

			template <typename T>
			static MemberVariable Create(const string& name)
			{
				using Type = typename VariableTraits<T>::Type;
				return { name, GetTypeInfo<Type>() };
			}
		};

		struct MemberFunction
		{
		public:
			string name;
			const BaseType* returnType;
			vector<const BaseType*> paramTypes;

			template <typename T>
			static MemberFunction Create(const string& name)
			{
				using FuncTraits = FunctionTraits<T>;
				if constexpr (FuncTraits::Args::size > 0)
				{
					return
					{
						name,
						GetTypeInfo<typename FuncTraits::ReturnType>(),
						GetParamTypes<typename FuncTraits::Args>(std::make_index_sequence<std::tuple_size_v<typename FuncTraits::Args>>())
					};
				}
				else
				{
					return
					{
						name,
						GetTypeInfo<typename FuncTraits::ReturnType>(),
						{}
					};
				}
			}

		private:
			// 从模板类型列表转换为BaseType指针vector数组
			template <typename Params, size_t... I>
			static vector<const BaseType*> GetParamTypes(std::index_sequence<I...>)
			{
				// 不定参数模板展开
				return { GetTypeInfo<typename std::tuple_element<I, Params>::type>()... };
			}
		};

		class ClassType : public BaseType
		{
			template <typename T>
			friend class ClassFactory;

		public:
			ClassType() : BaseType("Undefined-Class", Type::Class) {}
			ClassType(const string& name) : BaseType(name, Type::Class) {}

			void AddVariable(MemberVariable&& var)
			{
				mVariables.emplace_back(std::move(var));
			}

			void AddFunction(MemberFunction&& func)
			{
				mFunctions.emplace_back(std::move(func));
			}

			auto& GetVariables() const
			{
				return mVariables;
			}

			auto& GetFunctions() const
			{
				return mFunctions;
			}

		private:
			vector<MemberVariable> mVariables;
			vector<MemberFunction> mFunctions;
		};
	}
}