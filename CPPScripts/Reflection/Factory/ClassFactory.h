#pragma once
#include "../../pubh.h"
#include "../Type/ClassType.h"
#include "../TypeMap.h"

namespace ZXEngine
{
	namespace Reflection
	{
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

			void Register(const std::string& name)
			{
				mTypeInfo.mName = name;
			}

			template <typename MemberVariableType>
			void AddVariable(const string& name, MemberVariableType type)
			{
				mTypeInfo.AddVariable(MemberVariable<T, MemberVariableType>::Create(name, type));
			}

			template <typename MemberFunctionType>
			void AddFunction(const string& name, MemberFunctionType type)
			{
				mTypeInfo.AddFunction(MemberFunction::Create<MemberFunctionType>(name));
			}

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
	}
}