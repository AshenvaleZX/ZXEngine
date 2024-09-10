#pragma once
#include "../../pubh.h"
#include "../Type/NumericType.h"
#include "../TypeMap.h"

namespace ZXEngine
{
	namespace Reflection
	{
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

			NumericFactory(NumericType&& typeInfo) : mTypeInfo(std::move(typeInfo)) {}
		};
	}
}