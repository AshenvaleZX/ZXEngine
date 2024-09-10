#pragma once
#include "Factory.h"
#include "TypeMap.h"
#include "Type/EnumType.h"
#include "Type/NumericType.h"

namespace ZXEngine
{
	namespace Reflection
	{
		template <typename T>
		auto& Register()
		{
			return Factory<T>::GetFactory();
		}
	}
}