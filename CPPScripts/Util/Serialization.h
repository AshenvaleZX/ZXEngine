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
			struct SerializeHelper
			{
				static json SerializeToJson(const T& object)
				{
					json data;
					auto typeInfo = Reflection::Static::Reflect(object);

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
								data[field.GetName()] = SerializeHelper<type>::SerializeToJson(member);
							}
						}
					);

					return data;
				}
			};

			// 特化版本，用于处理string
			template <>
			struct SerializeHelper<string>
			{
				static json SerializeToJson(const string& object)
				{
					return object;
				}
			};
		}

		template <typename T>
		static string Serialize(const T& object)
		{
			json data = Internal::SerializeHelper<T>::SerializeToJson(object);
			return data.dump(4);
		}

		namespace Internal
		{
			template <typename T>
			struct DeserializeHelperV1
			{
				static T DeserializeFromJson(json data)
				{
					T object{};

					auto typeInfo = Reflection::Static::Reflect<T>();

					typeInfo.TraverseMemberVariableAndDo
					(
						[&](auto& field)
						{
							if (!data[field.GetName()].is_null())
							{
								auto& member = field.Invoke(object);

								using type = typename std::remove_cv_t<std::remove_reference_t<decltype(field)>>::Type;

								if constexpr (std::is_fundamental_v<type>)
								{
									member = data[field.GetName()];
								}
								else
								{
									member = DeserializeHelperV1<type>::DeserializeFromJson(data[field.GetName()]);
								}
							}
						}
					);

					return object;
				}
			};

			// 特化版本，用于处理string
			template <>
			struct DeserializeHelperV1<string>
			{
				static string DeserializeFromJson(json data)
				{
					return data.get<string>();
				}
			};
		}

		template <typename T>
		static T DeserializeFromJson(json data)
		{
			return Internal::DeserializeHelperV1<T>::DeserializeFromJson(data);
		}

		template <typename T>
		static T Deserialize(const string& data)
		{
			json jsonData = json::parse(data);
			return DeserializeFromJson<T>(jsonData);
		}

		namespace Internal
		{
			template <typename T>
			struct DeserializeHelperV2
			{
				static void DeserializeFromJson(T& object, json data)
				{
					auto typeInfo = Reflection::Static::Reflect<T>();

					typeInfo.TraverseMemberVariableAndDo
					(
						[&](auto& field)
						{
							if (!data[field.GetName()].is_null())
							{
								auto& member = field.Invoke(object);

								using type = typename std::remove_cv_t<std::remove_reference_t<decltype(field)>>::Type;

								if constexpr (std::is_fundamental_v<type>)
								{
									member = data[field.GetName()];
								}
								else
								{
									DeserializeHelperV2<type>::DeserializeFromJson(member, data[field.GetName()]);
								}
							}
						}
					);
				}
			};

			// 特化版本，用于处理string
			template <>
			struct DeserializeHelperV2<string>
			{
				static void DeserializeFromJson(string& object, json data)
				{
					object = data.get<string>();
				}
			};
		}

		template <typename T>
		static void DeserializeFromJson(T& object, json data)
		{
			Internal::DeserializeHelperV2<T>::DeserializeFromJson(object, data);
		}

		template <typename T>
		static void Deserialize(T& object, const string& data)
		{
			json jsonData = json::parse(data);
			DeserializeFromJson(object, jsonData);
		}
	}
}