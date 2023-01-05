#include "RenderQueue.h"
#include "GameObject.h"

namespace ZXEngine
{
	void RenderQueue::AddRenderer(MeshRenderer* meshRenderer)
	{
		renderers.push_back(meshRenderer);
	}

	vector<MeshRenderer*> RenderQueue::GetRenderers()
	{
		return renderers;
	}

	void RenderQueue::ClearRenderer()
	{
		renderers.clear();
	}

	void RenderQueue::Sort(Camera* camera)
	{
		sort(renderers.begin(), renderers.end(), [camera](MeshRenderer* a, MeshRenderer* b)->bool {
			auto aPos = a->gameObject->GetComponent<Transform>()->GetPosition();
			auto bPos = b->gameObject->GetComponent<Transform>()->GetPosition();
			auto cPos = camera->gameObject->GetComponent<Transform>()->GetPosition();
			auto aDis = Math::Distance(aPos, cPos);
			auto bDis = Math::Distance(bPos, cPos);
			return aDis < bDis;
		});
	}
}