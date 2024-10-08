#pragma once
#include "Factory.h"

// Registration Tool
#define ZXENGINE_CAT_IMPL(a, b) a##b
#define ZXENGINE_CAT(a, b) ZXENGINE_CAT_IMPL(a, b)

#define ZXENGINE_DYNAMIC_REFLECTION_REGISTRATION                                                          \
static void ZXEngineDynamicReflectionRegisterFunction();                                                  \
namespace                                                                                                 \
{                                                                                                         \
    struct ZXEngineDynamicReflectionRegister                                                              \
    {                                                                                                     \
        ZXEngineDynamicReflectionRegister()                                                               \
        {                                                                                                 \
            ZXEngineDynamicReflectionRegisterFunction();                                                  \
        }                                                                                                 \
    };                                                                                                    \
}                                                                                                         \
static const ZXEngineDynamicReflectionRegister ZXENGINE_CAT(ZXEngineDynamicReflectionRegister, __LINE__); \
static void ZXEngineDynamicReflectionRegisterFunction()

namespace ZXEngine
{
	namespace Reflection
	{
		namespace Dynamic
		{
			template <typename T>
			auto& Register()
			{
				return Factory<T>::GetFactory();
			}
		}
	}
}