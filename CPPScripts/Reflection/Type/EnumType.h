#pragma once
#include "BaseType.h"

namespace ZXEngine
{
	namespace Reflection
	{
		class EnumType : public BaseType
		{
			template <typename T>
			friend class EnumFactory;

		public:
			struct Item
			{
				int value;
				string name;
			};
		
		public:
			EnumType() : BaseType("Undefined-Enum", Type::Enum) {}
			EnumType(const string& name) : BaseType(name, Type::Enum) {}

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