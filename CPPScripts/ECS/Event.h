#pragma once
#include "../pubh.h"

namespace ZXEngine
{
	namespace ECS
	{
		template<typename T>
		class EventStaging
		{
		public:
			static bool Has()
			{
				return mEvent.has_value();
			}

			static void Set(const T& t)
			{
				mEvent = t;
			}

			static void Set(T&& t)
			{
				mEvent = std::move(t);
			}

			static T& Get()
			{
				return mEvent.value();
			}

			static void Clear()
			{
				mEvent.reset();
			}
			
		private:
			inline static std::optional<T> mEvent = std::nullopt;
		};

		template<typename T>
		class EventReader
		{
		public:
			bool Has() const
			{
				return EventStaging<T>::Has();
			}

			T Read()
			{
				return EventStaging<T>::Get();
			}

			const T& ReadConstRef()
			{
				return EventStaging<T>::Get();
			}

			void Clear()
			{
				EventStaging<T>::Clear();
			}
		};

		class Event
		{
			friend class World;
			template<typename T>
			friend class EventWriter;
		public:
			template<typename T>
			auto Reader();

			template<typename T>
			auto Writer();

		private:
			vector<void(*)(void)> mRemoveFuncs;
			vector<void(*)(void)> mDoRemoveFuncs;
			vector<std::function<void(void)>> mAddFuncs;

			void AddAllEvents()
			{
				for (auto& func : mAddFuncs)
				{
					func();
				}
				mAddFuncs.clear();
			}

			void RemoveAllEvents()
			{
				for (auto& func : mDoRemoveFuncs)
				{
					func();
				}
				mDoRemoveFuncs = mRemoveFuncs;
				mRemoveFuncs.clear();
			}
		};

		template<typename T>
		class EventWriter
		{
		public:
			EventWriter(Event& event) : 
				mEvent(event) 
			{}

			void Write(const T& t)
			{
				mEvent.mAddFuncs.push_back([=]()
				{
					EventStaging<T>::Set(t);
				});

				mEvent.mRemoveFuncs.push_back([]()
				{
					EventStaging<T>::Clear();
				});
			}

		private:
			Event& mEvent;
		};

		template <typename T>
		auto Event::Reader()
		{
			return EventReader<T>{};
		}

		template <typename T>
		auto Event::Writer()
		{
			return EventWriter<T>{*this};
		}
	}
}