#pragma once
#include "../pubh.h"

namespace ZXEngine
{
	namespace Reflection
	{
		class Argument
		{
			enum class FundamentalType
			{
				None,
				Bool,
				Char,
				Float,
				Double,
				Int8,
				Int16,
				Int32,
				Int64,
				UInt8,
				UInt16,
				UInt32,
				UInt64,
			};

		public:
			Argument() : mData(nullptr), mType(FundamentalType::None) {}
			Argument(const Argument& other) : mData(other.mData), mType(other.mType) {}
			Argument(Argument&& other) noexcept : mData(other.mData), mType(other.mType) {}

			template <typename T, typename std::enable_if<!std::is_same<Argument, typename std::decay<T>::type>::value, int>::type = 0>
			Argument(T& value) : mData(&value)
			{
				if constexpr (std::is_same_v<T, bool>)
					mType = FundamentalType::Bool;
				else if constexpr (std::is_same_v<T, char>)
					mType = FundamentalType::Char;
				else if constexpr (std::is_same_v<T, float>)
					mType = FundamentalType::Float;
				else if constexpr (std::is_same_v<T, double>)
					mType = FundamentalType::Double;
				else if constexpr (std::is_same_v<T, int8_t>)
					mType = FundamentalType::Int8;
				else if constexpr (std::is_same_v<T, int16_t>)
					mType = FundamentalType::Int16;
				else if constexpr (std::is_same_v<T, int32_t>)
					mType = FundamentalType::Int32;
				else if constexpr (std::is_same_v<T, int64_t>)
					mType = FundamentalType::Int64;
				else if constexpr (std::is_same_v<T, uint8_t>)
					mType = FundamentalType::UInt8;
				else if constexpr (std::is_same_v<T, uint16_t>)
					mType = FundamentalType::UInt16;
				else if constexpr (std::is_same_v<T, uint32_t>)
					mType = FundamentalType::UInt32;
				else if constexpr (std::is_same_v<T, uint64_t>)
					mType = FundamentalType::UInt64;
				else
					mType = FundamentalType::None;
			}

			template <typename T, typename std::enable_if<!std::is_same<Argument, typename std::decay<T>::type>::value, int>::type = 0>
			Argument(const T& value) : mData(&value)
			{
				if constexpr (std::is_same_v<T, bool>)
					mType = FundamentalType::Bool;
				else if constexpr (std::is_same_v<T, char>)
					mType = FundamentalType::Char;
				else if constexpr (std::is_same_v<T, float>)
					mType = FundamentalType::Float;
				else if constexpr (std::is_same_v<T, double>)
					mType = FundamentalType::Double;
				else if constexpr (std::is_same_v<T, int8_t>)
					mType = FundamentalType::Int8;
				else if constexpr (std::is_same_v<T, int16_t>)
					mType = FundamentalType::Int16;
				else if constexpr (std::is_same_v<T, int32_t>)
					mType = FundamentalType::Int32;
				else if constexpr (std::is_same_v<T, int64_t>)
					mType = FundamentalType::Int64;
				else if constexpr (std::is_same_v<T, uint8_t>)
					mType = FundamentalType::UInt8;
				else if constexpr (std::is_same_v<T, uint16_t>)
					mType = FundamentalType::UInt16;
				else if constexpr (std::is_same_v<T, uint32_t>)
					mType = FundamentalType::UInt32;
				else if constexpr (std::is_same_v<T, uint64_t>)
					mType = FundamentalType::UInt64;
				else
					mType = FundamentalType::None;
			}

			template <typename T, typename std::enable_if_t<std::is_fundamental_v<T>, int> = 0>
			T Get() const
			{
				if (mType == FundamentalType::Bool)
					return static_cast<const T>(*static_cast<const bool*>(mData));
				else if (mType == FundamentalType::Char)
					return static_cast<const T>(*static_cast<const char*>(mData));
				else if (mType == FundamentalType::Float)
					return static_cast<const T>(*static_cast<const float*>(mData));
				else if (mType == FundamentalType::Double)
					return static_cast<const T>(*static_cast<const double*>(mData));
				else if (mType == FundamentalType::Int8)
					return static_cast<const T>(*static_cast<const int8_t*>(mData));
				else if (mType == FundamentalType::Int16)
					return static_cast<const T>(*static_cast<const int16_t*>(mData));
				else if (mType == FundamentalType::Int32)
					return static_cast<const T>(*static_cast<const int32_t*>(mData));
				else if (mType == FundamentalType::Int64)
					return static_cast<const T>(*static_cast<const int64_t*>(mData));
				else if (mType == FundamentalType::UInt8)
					return static_cast<const T>(*static_cast<const uint8_t*>(mData));
				else if (mType == FundamentalType::UInt16)
					return static_cast<const T>(*static_cast<const uint16_t*>(mData));
				else if (mType == FundamentalType::UInt32)
					return static_cast<const T>(*static_cast<const uint32_t*>(mData));
				else if (mType == FundamentalType::UInt64)
					return static_cast<const T>(*static_cast<const uint64_t*>(mData));
				else
					throw std::runtime_error("Type mismatch or unsupported fundamental type.");
			}

			template <typename T, typename std::enable_if_t<!std::is_fundamental_v<T>, int> = 0>
			const T& Get() const
			{
				return *static_cast<const T*>(mData);
			}

		private:
			const void* mData;
			FundamentalType mType;
		};
	}
}