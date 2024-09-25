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

			template <typename T1>
			void AddVariable(const string& name, T1 type)
			{
				mTypeInfo.AddVariable(MemberVariable::Create<T1>(name));
			}

			template <typename T1>
			void AddFunction(const string& name, T1 type)
			{
				mTypeInfo.AddFunction(MemberFunction::Create<T1>(name));
			}

		private:
			ClassType mTypeInfo;
		};
	}
}