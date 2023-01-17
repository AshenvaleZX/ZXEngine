#pragma once
#include "pubh.h"

#define BatchMap map<unsigned int, vector<MeshRenderer*>>

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
		BatchMap GetBatches();
		void Clear();
		void Sort(Camera* camera);
		void Batch();

	private:
		vector<MeshRenderer*> renderers;
		BatchMap batches;
	};
}