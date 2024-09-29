#pragma once
#include "../pubh.h"
#include "Type/TypeInfo.h"

namespace ZXEngine
{
	namespace Reflection
	{
		namespace Dynamic
		{
			class TypeMap
			{
			public:
				static TypeMap& Get()
				{
					static TypeMap instance;
					return instance;
				}

			public:
				void RegisterType(const TypeInfo* type)
				{
					mTypeList.push_back(type);
				}

				const TypeInfo* GetType(const string& name)
				{
					for (auto iter : mTypeList)
					{
						if (iter->GetName() == name)
							return iter;
					}
					return nullptr;
				}

			private:
				vector<const TypeInfo*> mTypeList;

				TypeMap() = default;
			};

			inline const TypeInfo* GetTypeInfo(const string& name)
			{
				return TypeMap::Get().GetType(name);
			}
		}
	}
}