#pragma once
#include "../pubh.h"
#include "Type/BaseType.h"

namespace ZXEngine
{
	namespace Reflection
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
			void RegisterType(const BaseType* type)
			{
				mTypeList.push_back(type);
			}

			const BaseType* GetType(const string& name)
			{
				for (auto iter : mTypeList)
				{
					if (iter->GetName() == name)
						return iter;
				}
				return nullptr;
			}

		private:
			vector<const BaseType*> mTypeList;

			TypeMap() = default;
		};

		const BaseType* GetTypeInfo(const string& name)
		{
			return TypeMap::Get().GetType(name);
		}
	}
}