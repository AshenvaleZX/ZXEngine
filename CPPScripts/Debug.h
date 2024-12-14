#pragma once
#include <string>
#include <mutex>
#include <chrono>
#include <unordered_map>

// 条件编译调试代码开关
// #define ZX_DEBUG

namespace ZXEngine
{
	class Debug
	{
	/// <summary>
	/// 日志模块
	/// </summary>
	public:
		static void Log(const std::string& message);
		static void LogWarning(const std::string& message);
		static void LogError(const std::string& message);

		template<typename... Args>
		static void Log(const std::string& message, const Args&... args);
		template<typename... Args>
		static void LogWarning(const std::string& message, const Args&... args);
		template<typename... Args>
		static void LogError(const std::string& message, const Args&... args);

#ifdef ZX_DEBUG
		static int drawCallCount;
		static void Update();
#endif

	private:
		static std::mutex mWriteMutex;
		static void WriteToFile(const std::string& message);
		static void Replace(std::string& message, const std::string& from, const std::string& to);

		template<typename T>
		static std::string ToString(const T& t);
		template <std::size_t N>
		static std::string ToString(const char(&value)[N]);
		template<typename T>
		static void Format(std::string& message, const T& t);
		template<typename T, typename... Args>
		static void Format(std::string& message, const T& t, const Args&... args);


	/// <summary>
	/// 计时器模块
	/// </summary>
	public:
		static void PushTimer();
		static void PopTimer(const std::string& name = "");

		static void StartTimer(const std::string& name);
		static void EndTimer(const std::string& name);

	private:
		static size_t mTimerStackTop;
		static const size_t mTimerStackSize = 64;
		static std::chrono::steady_clock::time_point mTimerStack[mTimerStackSize];

		static std::unordered_map<std::string, std::chrono::steady_clock::time_point> mTimerMap;
	};

	// 基本模板函数(各种特化版本都匹配不上的时候就会用这个)
	template<typename T, typename = void>
	struct ToStringHelper 
	{
		static std::string Convert(const T&) 
		{
			return "<To String Failed>";
		}
	};

	// 特化版本，用于处理有ToString成员函数的类型
	template<typename T>
	struct ToStringHelper<T, std::void_t<decltype(std::declval<T>().ToString())>> 
	{
		static std::string Convert(const T& value) 
		{
			return value.ToString();
		}
	};

	// 特化版本，用于处理可以用std::to_string转换的类型
	template<typename T>
	struct ToStringHelper<T, std::enable_if_t<std::is_arithmetic_v<T>>> 
	{
		static std::string Convert(const T& value) 
		{
			return std::to_string(value);
		}
	};

	// 特化版本，用于处理enum和enum class
	template<typename T>
	struct ToStringHelper<T, std::enable_if_t<std::is_enum_v<T>>>
	{
		static std::string Convert(const T& value)
		{
			return std::to_string(static_cast<std::underlying_type_t<T>>(value));
		}
	};

	// 特化版本，用于处理string
	template<>
	struct ToStringHelper<std::string>
	{
		static std::string Convert(const std::string& value)
		{
			return value;
		}
	};

	// 特化版本，用于处理std::string_view
	template<>
	struct ToStringHelper<std::string_view>
	{
		static std::string Convert(const std::string_view& value)
		{
			return std::string(value);
		}
	};

	// 特化版本，用于处理char*
	template<>
	struct ToStringHelper<const char*>
	{
		static std::string Convert(const char* value)
		{
			return value;
		}
	};

	template<typename... Args>
	void Debug::Log(const std::string& message, const Args&... args)
	{
		std::string msg = message;
		Format(msg, args...);
		Log(msg);
	}

	template<typename... Args>
	void Debug::LogWarning(const std::string& message, const Args&... args)
	{
		std::string msg = message;
		Format(msg, args...);
		LogWarning(msg);
	}

	template<typename... Args>
	void Debug::LogError(const std::string& message, const Args&... args)
	{
		std::string msg = message;
		Format(msg, args...);
		LogError(msg);
	}

	template<typename T>
	std::string Debug::ToString(const T& t)
	{
		return ToStringHelper<T>::Convert(t);
	}

	// 重载版本，用于处理const char[N]类型
	// 如果写一个const char[N]特化版本的ToStringHelper会被基本模板先匹配，所以这种情况只能重载ToString函数而不是新增特化版本的ToStringHelper
	template<std::size_t N>
	std::string Debug::ToString(const char(&value)[N])
	{
		return std::string(value);
	}

	template<typename T>
	void Debug::Format(std::string& message, const T& t)
	{
		Replace(message, "%s", ToString(t));
	}

	template<typename T, typename... Args>
	void Debug::Format(std::string& message, const T& t, const Args&... args)
	{
		Replace(message, "%s", ToString(t));
		Format(message, args...);
	}
}