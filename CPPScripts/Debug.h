#pragma once
#include <string>
#include <mutex>
#include <chrono>
#include <unordered_map>

// ����������Դ��뿪��
// #define ZX_DEBUG

namespace ZXEngine
{
	class Debug
	{
	/// <summary>
	/// ��־ģ��
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
	/// ��ʱ��ģ��
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

	// ����ģ�庯��(�����ػ��汾��ƥ�䲻�ϵ�ʱ��ͻ������)
	template<typename T, typename = void>
	struct ToStringHelper 
	{
		static std::string Convert(const T&) 
		{
			return "<To String Failed>";
		}
	};

	// �ػ��汾�����ڴ�����ToString��Ա����������
	template<typename T>
	struct ToStringHelper<T, std::void_t<decltype(std::declval<T>().ToString())>> 
	{
		static std::string Convert(const T& value) 
		{
			return value.ToString();
		}
	};

	// �ػ��汾�����ڴ��������std::to_stringת��������
	template<typename T>
	struct ToStringHelper<T, std::enable_if_t<std::is_arithmetic_v<T>>> 
	{
		static std::string Convert(const T& value) 
		{
			return std::to_string(value);
		}
	};

	// �ػ��汾�����ڴ���enum��enum class
	template<typename T>
	struct ToStringHelper<T, std::enable_if_t<std::is_enum_v<T>>>
	{
		static std::string Convert(const T& value)
		{
			return std::to_string(static_cast<std::underlying_type_t<T>>(value));
		}
	};

	// �ػ��汾�����ڴ���string
	template<>
	struct ToStringHelper<std::string>
	{
		static std::string Convert(const std::string& value)
		{
			return value;
		}
	};

	// �ػ��汾�����ڴ���std::string_view
	template<>
	struct ToStringHelper<std::string_view>
	{
		static std::string Convert(const std::string_view& value)
		{
			return std::string(value);
		}
	};

	// �ػ��汾�����ڴ���char*
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

	// ���ذ汾�����ڴ���const char[N]����
	// ���дһ��const char[N]�ػ��汾��ToStringHelper�ᱻ����ģ����ƥ�䣬�����������ֻ������ToString���������������ػ��汾��ToStringHelper
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