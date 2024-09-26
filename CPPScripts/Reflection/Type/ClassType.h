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
			// ���Ա������ַ
			MemberVariableType mAddr = nullptr;
			using VariableType = typename VariableTraits<MemberVariableType>::Type;

			void Set(void* obj, Argument value) const override
			{
				auto c = static_cast<Class*>(obj);
				c->*mAddr = value.Get<VariableType>();
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
						GetParamTypes<typename FuncTraits::Args>(std::make_index_sequence<std::tuple_size_v<typename FuncTraits::Args>>())
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
			// ��ģ�������б�ת��ΪBaseTypeָ��vector����
			template <typename Params, size_t... I>
			static vector<const BaseType*> GetParamTypes(std::index_sequence<I...>)
			{
				// ��������ģ��չ��
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

		private:
			vector<unique_ptr<MemberVariableBase>> mVariables;
			vector<unique_ptr<MemberFunction>> mFunctions;
		};
	}
}