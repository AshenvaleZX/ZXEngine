#pragma once
#include "pubh.h"

#define RendererList vector<MeshRenderer*>
#define BatchMap map<uint32_t, RendererList>

namespace ZXEngine
{
	enum class RenderSortType
	{
		None,
		FrontToBack,
		BackToFront,
	};

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
		void Sort(Camera* camera, RenderSortType type = RenderSortType::None);
		void Batch();

	private:
		RendererList renderers;
		BatchMap batches;
		// ���ں�������ʱRenderer�����poolֻ����ɾ
		// ���Ҳ��������κ�delete����Ϊ�������õ������ط��Ķ���ʵ��
		RendererList temporaryRendererPool;
		// ��ǰʹ������
		int rendererPoolIdx = 0;
		// ��¼��̬������������ʱMesh��ÿ֡delete
		list<StaticMesh*> temporaryMeshPool;

		void DynamicBatch(RendererList& batchRenderers);
		MeshRenderer* MergeMeshs(RendererList& batchRenderers);
		MeshRenderer* GetTemporaryRenderer();
	};
}