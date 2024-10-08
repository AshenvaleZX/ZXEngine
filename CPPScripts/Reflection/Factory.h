#pragma once
#include <type_traits>
#include "TypeMap.h"
#include "Type/VoidType.h"
#include "Type/NumericType.h"
#include "Type/EnumType.h"
#include "Type/ClassType.h"

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

#include "Type/ClassType.h"

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