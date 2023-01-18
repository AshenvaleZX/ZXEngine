#pragma once
#include "pubh.h"

#define RendererList vector<MeshRenderer*>
#define BatchMap map<unsigned int, RendererList>

namespace ZXEngine
{
	class Camera;
	class StaticMesh;
	class MeshRenderer;
	class RenderObject;
	class RenderQueue
	{
	public:
		RenderQueue() {};
		~RenderQueue() {};

		int queue = 0;
		void AddRenderer(MeshRenderer* meshRenderer);
		RendererList GetRenderers();
		BatchMap GetBatches();
		void Clear();
		void Sort(Camera* camera);
		void Batch();

	private:
		RendererList renderers;
		BatchMap batches;
		// 用于合批的临时Renderer，这个pool只增不删
		// 并且不可以做任何delete，因为都是引用的其他地方的对象实例
		RendererList temporaryRendererPool;
		// 当前使用数量
		int rendererPoolIdx = 0;
		// 记录动态合批产生的临时Mesh，每帧delete
		list<StaticMesh*> temporaryMeshPool;

		void DynamicBatch(RendererList& batchRenderers);
		MeshRenderer* MergeMeshs(RendererList& batchRenderers);
		MeshRenderer* GetTemporaryRenderer();
	};
}