#pragma once
#include "pubh.h"

namespace ZXEngine
{
	class Camera;
	class MeshRenderer;
	class RenderQueue
	{
	public:
		RenderQueue() {};
		~RenderQueue() {};

		int queue = 0;
		void AddRenderer(MeshRenderer* meshRenderer);
		vector<MeshRenderer*> GetRenderers();
		void ClearRenderer();
		void Sort(Camera* camera);

	private:
		vector<MeshRenderer*> renderers;
	};
}