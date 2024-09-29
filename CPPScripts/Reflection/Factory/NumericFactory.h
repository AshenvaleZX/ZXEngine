#pragma once
#include "../../pubh.h"
#include "../Type/NumericType.h"
#include "../TypeMap.h"

namespace ZXEngine
{
	namespace Reflection
	{
		namespace Dynamic
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

				// 不允许从外部构造和销毁
				NumericFactory() = default;
				~NumericFactory() = default;

				NumericFactory(NumericFactory&&) = delete;
				NumericFactory& operator=(NumericFactory&&) = delete;

				NumericFactory(const NumericFactory&) = delete;
				NumericFactory& operator=(const NumericFactory&) = delete;

				NumericFactory(NumericType&& typeInfo) : mTypeInfo(std::move(typeInfo)) {}
			};
		}
	}
}