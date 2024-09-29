#pragma once
#include "../../pubh.h"
#include "../Type/VoidType.h"
#include "../TypeMap.h"

namespace ZXEngine
{
	namespace Reflection
	{
		namespace Dynamic
		{
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
		}
	}
}