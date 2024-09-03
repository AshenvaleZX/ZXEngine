#pragma once
#include "../pubh.h"
#include "../Reflection/StaticReflection.h"

namespace ZXEngine
{
	namespace Serialization
	{
		namespace Internal
		{
			template <typename T>
			static json SerializeToJson(const T& object)
			{
				json data;
				auto typeInfo = Reflection::Reflect(object);

				data["Type"] = typeInfo.GetName();

				typeInfo.TraverseMemberVariableAndDo
				(
					[&](auto& field)
					{
						auto& member = field.Invoke(object);

						using type = typename std::remove_cv_t<std::remove_reference_t<decltype(field)>>::Type;

						if constexpr (std::is_fundamental_v<type>)
						{
							data[field.GetName()] = member;
						}
						else
						{
							data[field.GetName()] = SerializeToJson(member);
						}
					}
				);

				return data;
			}
		}

		template <typename T>
		static string Serialize(const T& object)
		{
			json data;
			auto typeInfo = Reflection::Reflect(object);

			data["Type"] = typeInfo.GetName();

			typeInfo.TraverseMemberVariableAndDo
			(
				[&](auto& field)
				{
					auto& member = field.Invoke(object);

					using type = typename std::remove_cv_t<std::remove_reference_t<decltype(field)>>::Type;

					if constexpr (std::is_fundamental_v<type>)
					{
						data[field.GetName()] = member;
					}
					else
					{
						data[field.GetName()] = Internal::SerializeToJson(member);
					}
				}
			);

			return data.dump(4);
		}
	}
}