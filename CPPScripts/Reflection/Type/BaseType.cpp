#include "BaseType.h"
#include "EnumType.h"
#include "NumericType.h"
#include "ClassType.h"

namespace ZXEngine
{
	namespace Reflection
	{
		const EnumType* BaseType::AsEnum() const
		{
			if (mType == Type::Enum)
			{
				return static_cast<const EnumType*>(this);
			}
			else
			{
				Debug::LogError("BaseType convert failed, type is not Enum.");
				return nullptr;
			}
		}

		const NumericType* BaseType::AsNumeric() const
		{
			if (mType == Type::Numeric)
			{
				return static_cast<const NumericType*>(this);
			}
			else
			{
				Debug::LogError("BaseType convert failed, type is not Numeric.");
				return nullptr;
			}
		}

		const ClassType* BaseType::AsClass() const
		{
			if (mType == Type::Class)
			{
				return static_cast<const ClassType*>(this);
			}
			else
			{
				Debug::LogError("BaseType convert failed, type is not Class.");
				return nullptr;
			}
		}
	}
}