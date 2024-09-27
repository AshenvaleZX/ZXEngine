#pragma once
#include "../../pubh.h"
#include "BaseType.h"
#include "../VariableTraits.h"
#include "../FunctionTraits.h"
#include "../Argument.h"

namespace ZXEngine
{
	namespace Reflection
	{
		struct MemberVariableBase
		{
			template <typename T>
			friend class ClassFactory;

		public:
			string name;
			const BaseType* type = nullptr;

			virtual void Set(void* obj, Argument value) const {};
			virtual std::any Get(void* obj) const { return std::any(); }

		private:
			template <typename MemberVariableType>
			static unique_ptr<MemberVariableBase> Create(const string& name)
			{
				using Type = typename VariableTraits<MemberVariableType>::Type;
				return make_unique<MemberVariableBase>(name, GetTypeInfo<Type>());
			}
		};

		template <typename Class, typename MemberVariableType>
		struct MemberVariable : public MemberVariableBase
		{
			template <typename T>
			friend class ClassFactory;

		public:
			// 类成员变量地址
			MemberVariableType mAddr = nullptr;
			using VariableType = typename VariableTraits<MemberVariableType>::Type;

			void Set(void* obj, Argument value) const override
			{
				auto c = static_cast<Class*>(obj);
				c->*mAddr = value.Get<VariableType>();
			}

			std::any Get(void* obj) const override
			{
				auto c = static_cast<Class*>(obj);
				return c->*mAddr;
			}

		private:
			static unique_ptr<MemberVariable> Create(const string& name, MemberVariableType addr)
			{
				using Type = typename VariableTraits<MemberVariableType>::Type;

				auto mv = make_unique<MemberVariable<Class, MemberVariableType>>();
				mv->name = name;
				mv->type = GetTypeInfo<Type>();
				mv->mAddr = addr;

				return mv;
			}
		};

		struct MemberFunction
		{
			template <typename T>
			friend class ClassFactory;

		public:
			string name;
			const BaseType* returnType;
			vector<const BaseType*> paramTypes;

			MemberFunction(const string& name, const BaseType* returnType, vector<const BaseType*> paramTypes) : 
				name(name), 
				returnType(returnType), 
				paramTypes(paramTypes)
			{}

		private:
			template <typename MemberFunctionType>
			static unique_ptr<MemberFunction> Create(const string& name)
			{
				using FuncTraits = FunctionTraits<MemberFunctionType>;
				if constexpr (FuncTraits::Args::size > 0)
				{
					return make_unique<MemberFunction>
					(
						name,
						GetTypeInfo<typename FuncTraits::ReturnType>(),
						GetParamTypes<typename FuncTraits::Args>(std::make_index_sequence<FuncTraits::Args::size>())
					);
				}
				else
				{
					return make_unique<MemberFunction>
					(
						name,
						GetTypeInfo<typename FuncTraits::ReturnType>(),
						vector<const BaseType*>{}
					);
				}
			}

		private:
			// 从模板类型列表转换为BaseType指针vector数组
			template <typename Params, size_t... I>
			static vector<const BaseType*> GetParamTypes(std::index_sequence<I...>)
			{
				// 将TypeList转换为Tuple
				using TupleType = typename TypeListToTuple<Params>::type;
				// 不定参数模板展开
				return { GetTypeInfo<typename std::tuple_element<I, TupleType>::type>()... };
			}
		};

		class ClassType : public BaseType
		{
			template <typename T>
			friend class ClassFactory;

		public:
			ClassType() : BaseType("Undefined-Class", Type::Class) {}
			ClassType(const string& name) : BaseType(name, Type::Class) {}

			void AddVariable(unique_ptr<MemberVariableBase> var)
			{
				mVariables.emplace_back(std::move(var));
			}

			void AddFunction(unique_ptr<MemberFunction> func)
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

			void SetVariable(void* obj, const string& name, Argument value) const
			{
				for (auto& var : mVariables)
				{
					if (var->name == name)
					{
						var->Set(obj, value);
						return;
					}
				}
			}

			std::any GetVariable(void* obj, const string& name) const
			{
				for (auto& var : mVariables)
				{
					if (var->name == name)
					{
						return var->Get(obj);
					}
				}
				return std::any();
			}

		private:
			vector<unique_ptr<MemberVariableBase>> mVariables;
			vector<unique_ptr<MemberFunction>> mFunctions;
		};
	}
}