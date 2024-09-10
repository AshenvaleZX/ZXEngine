#pragma once
#include "../../pubh.h"

namespace ZXEngine
{
	namespace Reflection
	{
		class EnumType;
		class NumericType;
		class ClassType;
		class BaseType
		{
		public:
			enum class Type
			{
				Numeric,
				Enum,
				Class,
			};

		public:
			BaseType(const string& name, Type type) : mName(name), mType(type) {}
			virtual ~BaseType() = default;

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