#pragma once
#include "pubh.h"

namespace ZXEngine
{
	class MeshRenderer;
	class RenderQueue
	{
	public:
		RenderQueue() {};
		~RenderQueue() {};

		int queue = 0;
		void AddRenderer(MeshRenderer* meshRenderer);

	private:
		vector<MeshRenderer*> renderers;
	};
}