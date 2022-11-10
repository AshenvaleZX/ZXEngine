#pragma once
#include "pubh.h"

namespace ZXEngine
{
	class Camera;
	class RenderPass
	{
	public:
		RenderPass() {};
		~RenderPass() {};

		virtual void Render(Camera* camera) = 0;
	};
}