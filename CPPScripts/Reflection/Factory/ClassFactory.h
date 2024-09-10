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

			void Register(const std::string& name)
			{
				mTypeInfo.mName = name;
			}

			template <typename T1>
			void AddVariable(const string& name)
			{
				mTypeInfo.AddVariable(MemberVariable::Create<T1>(name));
			}

			template <typename T1>
			void AddFunction(const string& name)
			{
				mTypeInfo.AddFunction(MemberFunction::Create<T1>(name));
			}

		private:
			ClassType mTypeInfo;
		};
	}
}