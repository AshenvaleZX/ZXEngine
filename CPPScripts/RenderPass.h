#pragma once
#include "pubh.h"

namespace ZXEngine
{
	class RenderPass
	{
	public:
		RenderPass() {};
		~RenderPass() {};

		virtual void Render() = 0;
	};
}