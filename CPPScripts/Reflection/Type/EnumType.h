#pragma once
#include "TypeInfo.h"

namespace ZXEngine
{
	namespace Reflection
	{
		namespace Dynamic
		{
			class EnumType : public TypeInfo
			{
				template <typename T>
				friend class EnumFactory;

			public:
				struct Item
				{
					int value;
					string name;

					Item(int value, const string& name) : value(value), name(name) {}
				};

			public:
				EnumType() : TypeInfo("Undefined-Enum", Type::Enum) {}
				EnumType(const string& name) : TypeInfo(name, Type::Enum) {}

				template <typename T>
				void AddItem(const string& name, T value)
				{
					mItems.emplace_back(static_cast<int>(value), name);
				}

				auto& GetItems() const { return mItems; }

			private:
				vector<Item> mItems;
			};
		}
	}
}