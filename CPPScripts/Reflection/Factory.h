#pragma once
#include <type_traits>
#include "TypeMap.h"
#include "Type/VoidType.h"
#include "Type/NumericType.h"
#include "Type/EnumType.h"

#include "VariableTraits.h"
#include "FunctionTraits.h"
#include "Argument.h"

namespace ZXEngine
{
	namespace Reflection
	{
		namespace Dynamic
		{
			/// ----------------------------------------------------------------
			/// DefaultFactory
			/// ----------------------------------------------------------------
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


			/// ----------------------------------------------------------------
			/// VoidFactory
			/// ----------------------------------------------------------------
			template<typename T>
			class VoidFactory
			{
			public:
				static VoidFactory& Get()
				{
					static VoidFactory instance;

					static bool isRegistered = false;
					if (!isRegistered)
					{
						TypeMap::Get().RegisterType(&instance.mTypeInfo);
						isRegistered = true;
					}

					return instance;
				}

			public:
				const VoidType& GetTypeInfo() const
				{
					return mTypeInfo;
				}

			private:
				VoidType mTypeInfo;

				// 不允许从外部构造和销毁
				VoidFactory() = default;
				~VoidFactory() = default;

				VoidFactory(VoidFactory&&) = delete;
				VoidFactory& operator=(VoidFactory&&) = delete;

				VoidFactory(const VoidFactory&) = delete;
				VoidFactory& operator=(const VoidFactory&) = delete;
			};


			/// ----------------------------------------------------------------
			/// NumericFactory
			/// ----------------------------------------------------------------
			template<typename T>
			class NumericFactory
			{
			public:
				static NumericFactory& Get()
				{
					static NumericFactory instance{ NumericType::Create<T>() };

					static bool isRegistered = false;
					if (!isRegistered)
					{
						TypeMap::Get().RegisterType(&instance.mTypeInfo);
						isRegistered = true;
					}

					return instance;
				}

			public:
				const NumericType& GetTypeInfo() const
				{
					return mTypeInfo;
				}

			private:
				NumericType mTypeInfo;

				// 不允许从外部构造和销毁
				NumericFactory() = default;
				~NumericFactory() = default;

				NumericFactory(NumericFactory&&) = delete;
				NumericFactory& operator=(NumericFactory&&) = delete;

				NumericFactory(const NumericFactory&) = delete;
				NumericFactory& operator=(const NumericFactory&) = delete;

				NumericFactory(NumericType&& typeInfo) : mTypeInfo(std::move(typeInfo)) {}
			};


			/// ----------------------------------------------------------------
			/// EnumFactory
			/// ----------------------------------------------------------------
			template<typename T>
			class EnumFactory
			{
			public:
				static EnumFactory& Get()
				{
					static EnumFactory instance;

					static bool isRegistered = false;
					if (!isRegistered)
					{
						TypeMap::Get().RegisterType(&instance.mTypeInfo);
						isRegistered = true;
					}

					return instance;
				}

			public:
				const EnumType& GetTypeInfo() const
				{
					return mTypeInfo;
				}

				EnumFactory& Register(const std::string& name)
				{
					mTypeInfo.mName = name;
					return *this;
				}

				template <typename E>
				EnumFactory& AddEnum(const std::string& name, E value)
				{
					mTypeInfo.AddItem(name, value);
					return *this;
				}

			private:
				EnumType mTypeInfo;

				// 不允许从外部构造和销毁
				EnumFactory() = default;
				~EnumFactory() = default;

				EnumFactory(EnumFactory&&) = delete;
				EnumFactory& operator=(EnumFactory&&) = delete;

				EnumFactory(const EnumFactory&) = delete;
				EnumFactory& operator=(const EnumFactory&) = delete;
			};


			/// ----------------------------------------------------------------
			/// ClassType
			/// ----------------------------------------------------------------
			struct MemberVariableBase
			{
				template <typename T>
				friend class ClassFactory;

			public:
				string name;
				const TypeInfo* type = nullptr;

				virtual void Set(void* obj, Argument value) const {};
				virtual std::any Get(void* obj) const { return std::any(); }

			private:
				template <typename MemberVariableType>
				static unique_ptr<MemberVariableBase> Create(const string& name);
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
				static unique_ptr<MemberVariable> Create(const string& name, MemberVariableType addr);
			};

			struct MemberFunction
			{
				template <typename T>
				friend class ClassFactory;

			public:
				string name;
				const TypeInfo* returnType;
				vector<const TypeInfo*> paramTypes;

				MemberFunction(const string& name, const TypeInfo* returnType, vector<const TypeInfo*> paramTypes) :
					name(name),
					returnType(returnType),
					paramTypes(paramTypes)
				{}

				template <typename MemberFunctionType>
				static unique_ptr<MemberFunction> Create(const string& name);

			private:
				// 从模板类型列表转换为TypeInfo指针vector数组
				template <typename Params, size_t... I>
				static vector<const TypeInfo*> GetParamTypes(std::index_sequence<I...>);
			};

			class ClassType : public TypeInfo
			{
				template <typename T>
				friend class ClassFactory;

			public:
				ClassType() : TypeInfo("Undefined-Class", Type::Class) {}
				ClassType(const string& name) : TypeInfo(name, Type::Class) {}

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


			/// ----------------------------------------------------------------
			/// ClassFactory
			/// ----------------------------------------------------------------
			template<typename T>
			class ClassFactory
			{
			public:
				static ClassFactory& Get()
				{
					static ClassFactory instance;

					static bool isRegistered = false;
					if (!isRegistered)
					{
						TypeMap::Get().RegisterType(&instance.mTypeInfo);
						isRegistered = true;
					}

					return instance;
				}

			public:
				const ClassType& GetTypeInfo() const
				{
					return mTypeInfo;
				}

				ClassFactory& Register(const std::string& name)
				{
					mTypeInfo.mName = name;
					return *this;
				}

				template <typename MemberVariableType>
				ClassFactory& AddVariable(const string& name, MemberVariableType type);

				template <typename MemberFunctionType>
				ClassFactory& AddFunction(const string& name, MemberFunctionType type);

			private:
				ClassType mTypeInfo;

				// 不允许从外部构造和销毁
				ClassFactory() = default;
				~ClassFactory() = default;

				ClassFactory(ClassFactory&&) = delete;
				ClassFactory& operator=(ClassFactory&&) = delete;

				ClassFactory(const ClassFactory&) = delete;
				ClassFactory& operator=(const ClassFactory&) = delete;
			};


			/// ----------------------------------------------------------------
			/// Factory
			/// ----------------------------------------------------------------
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


			/// ----------------------------------------------------------------
			/// GetTypeInfo<T>
			/// ----------------------------------------------------------------
			template <typename T>
			inline const TypeInfo* GetTypeInfo()
			{
				return &Factory<T>::GetFactory().GetTypeInfo();
			}

			template <typename MemberVariableType>
			unique_ptr<MemberVariableBase> MemberVariableBase::Create(const string& name)
			{
				using Type = typename VariableTraits<MemberVariableType>::Type;
				return make_unique<MemberVariableBase>(name, GetTypeInfo<Type>());
			}

			template <typename Class, typename MemberVariableType>
			unique_ptr<MemberVariable<Class, MemberVariableType>> MemberVariable<Class, MemberVariableType>::Create(const string& name, MemberVariableType addr)
			{
				using Type = typename VariableTraits<MemberVariableType>::Type;

				auto mv = make_unique<MemberVariable<Class, MemberVariableType>>();
				mv->name = name;
				mv->type = GetTypeInfo<Type>();
				mv->mAddr = addr;

				return mv;
			}

			template <typename MemberFunctionType>
			unique_ptr<MemberFunction> MemberFunction::Create(const string& name)
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
						vector<const TypeInfo*>{}
					);
				}
			}

			template <typename Params, size_t... I>
			vector<const TypeInfo*> MemberFunction::GetParamTypes(std::index_sequence<I...>)
			{
				// 将TypeList转换为Tuple
				using TupleType = typename TypeListToTuple<Params>::type;
				// 不定参数模板展开
				return { GetTypeInfo<typename std::tuple_element<I, TupleType>::type>()... };
			}
			
			template<typename T>
			template <typename MemberVariableType>
			ClassFactory<T>& ClassFactory<T>::AddVariable(const string& name, MemberVariableType type)
			{
				mTypeInfo.AddVariable(MemberVariable<T, MemberVariableType>::Create(name, type));
				return *this;
			}

			template<typename T>
			template <typename MemberFunctionType>
			ClassFactory<T>& ClassFactory<T>::AddFunction(const string& name, MemberFunctionType type)
			{
				mTypeInfo.AddFunction(MemberFunction::Create<MemberFunctionType>(name));
				return *this;
			}
		}
	}
}