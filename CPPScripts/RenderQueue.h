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
		list<MeshRenderer*> GetRenderers();
		void ClearRenderer();

	private:
		list<MeshRenderer*> renderers;
	};
}