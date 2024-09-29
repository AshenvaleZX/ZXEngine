#include "NumericType.h"

namespace ZXEngine
{
	namespace Reflection
	{
		namespace Dynamic
		{
			NumericType::NumericType(NumericType::Type type, bool isSigned) :
				TypeInfo(GetTypeName(type), TypeInfo::Type::Numeric),
				mType(type),
				mIsSigned(isSigned)
			{}

			NumericType::Type NumericType::GetType() const
			{
				return mType;
			}

			string NumericType::GetTypeName(NumericType::Type type)
			{
				switch (type)
				{
				case Type::Int8:
					return "Int8";
				case Type::Int16:
					return "Int16";
				case Type::Int32:
					return "Int32";
				case Type::Int64:
					return "Int64";
				case Type::UInt8:
					return "UInt8";
				case Type::UInt16:
					return "UInt16";
				case Type::UInt32:
					return "UInt32";
				case Type::UInt64:
					return "UInt64";
				case Type::Float:
					return "Float";
				case Type::Double:
					return "Double";
				default:
					return "Unknown";
				}
			}
		}
	}
}