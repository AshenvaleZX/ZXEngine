#pragma once
#include "BaseType.h"

namespace ZXEngine
{
	namespace Reflection
	{
		class NumericType : public BaseType
		{
		public:
			enum class Type
			{
				Unknown,
				Int8,
				Int16,
				Int32,
				Int64,
				UInt8,
				UInt16,
				UInt32,
				UInt64,
				Float,
				Double
			};

		public:
			NumericType(Type type, bool isSigned);
			Type GetType() const;

		private:
			Type mType;
			bool mIsSigned;

		public:
			template <typename T>
			static NumericType Create()
			{
				return NumericType(GetType<T>(), std::is_signed_v<T>);
			}

		private:
			static string GetTypeName(Type type);

			template <typename T>
			static Type GetType()
			{
				if constexpr (std::is_same_v<T, int8_t>)
					return Type::Int8;
				else if constexpr (std::is_same_v<T, int16_t>)
					return Type::Int16;
				else if constexpr (std::is_same_v<T, int32_t>)
					return Type::Int32;
				else if constexpr (std::is_same_v<T, int64_t>)
					return Type::Int64;
				else if constexpr (std::is_same_v<T, uint8_t>)
					return Type::UInt8;
				else if constexpr (std::is_same_v<T, uint16_t>)
					return Type::UInt16;
				else if constexpr (std::is_same_v<T, uint32_t>)
					return Type::UInt32;
				else if constexpr (std::is_same_v<T, uint64_t>)
					return Type::UInt64;
				else if constexpr (std::is_same_v<T, float>)
					return Type::Float;
				else if constexpr (std::is_same_v<T, double>)
					return Type::Double;
				else
					return Type::Unknown;
			}
		};
	}
}