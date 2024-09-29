#pragma once
#include "TypeInfo.h"

namespace ZXEngine
{
	namespace Reflection
	{
		namespace Dynamic
		{
			class VoidType : public TypeInfo
			{
			public:
				VoidType() : TypeInfo("Void", Type::Void) {}
			};
		}
	}
}