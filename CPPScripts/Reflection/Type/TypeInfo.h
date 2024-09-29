#pragma once
#include "../../pubh.h"

namespace ZXEngine
{
	namespace Reflection
	{
		namespace Dynamic
		{
			class EnumType;
			class NumericType;
			class ClassType;
			class TypeInfo
			{
			public:
				enum class Type
				{
					Numeric,
					Enum,
					Class,
					Void,
				};

			public:
				TypeInfo(const string& name, Type type) : mName(name), mType(type) {}
				virtual ~TypeInfo() = default;

				Type GetType() const { return mType; }
				string GetName() const { return mName; }

				const EnumType* AsEnum() const;
				const NumericType* AsNumeric() const;
				const ClassType* AsClass() const;

			protected:
				Type mType;
				string mName;
			};
		}
	}
}