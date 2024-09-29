#pragma once
#include "../../pubh.h"
#include "../Type/EnumType.h"
#include "../TypeMap.h"

namespace ZXEngine
{
	namespace Reflection
	{
		namespace Dynamic
		{
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
		}
	}
}