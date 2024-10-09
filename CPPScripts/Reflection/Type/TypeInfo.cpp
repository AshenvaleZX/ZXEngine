#include "TypeInfo.h"
#include "../Factory.h"

namespace ZXEngine
{
	namespace Reflection
	{
		namespace Dynamic
		{
			const EnumType* TypeInfo::AsEnum() const
			{
				if (mType == Type::Enum)
				{
					return static_cast<const EnumType*>(this);
				}
				else
				{
					Debug::LogError("Type convert failed, type is not Enum.");
					return nullptr;
				}
			}

			const NumericType* TypeInfo::AsNumeric() const
			{
				if (mType == Type::Numeric)
				{
					return static_cast<const NumericType*>(this);
				}
				else
				{
					Debug::LogError("Type convert failed, type is not Numeric.");
					return nullptr;
				}
			}

			const ClassType* TypeInfo::AsClass() const
			{
				if (mType == Type::Class)
				{
					return static_cast<const ClassType*>(this);
				}
				else
				{
					Debug::LogError("Type convert failed, type is not Class.");
					return nullptr;
				}
			}
		}
	}
}