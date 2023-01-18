#include "RenderQueue.h"
#include "GameObject.h"
#include "ProjectSetting.h"

namespace ZXEngine
{
	void RenderQueue::AddRenderer(MeshRenderer* meshRenderer)
	{
		renderers.push_back(meshRenderer);
	}

	RendererList RenderQueue::GetRenderers()
	{
		return renderers;
	}

	BatchMap RenderQueue::GetBatches()
	{
		return batches;
	}

	void RenderQueue::Clear()
	{
		renderers.clear();

		for (auto& batch : batches)
			batch.second.clear();
		batches.clear();
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

	void RenderQueue::Batch()
	{
		for (auto renderer : renderers)
		{
			batches[renderer->matetrial->shader->GetID()].push_back(renderer);
		}

		// 动态合批
		if (ProjectSetting::enableDynamicBatch)
		{
			rendererPoolIdx = 0;
			for (auto mesh : temporaryMeshPool)
				delete mesh;
			temporaryMeshPool.clear();

			for (auto& iter : batches)
				DynamicBatch(iter.second);
		}
	}

	void RenderQueue::DynamicBatch(RendererList& batchRenderers)
	{
		// 这里接受的参数是相同Shader的对象，再按材质分组
		map<string, RendererList> matMap;
		for (auto renderer : batchRenderers)
			matMap[renderer->matetrial->path].push_back(renderer);

		RendererList newList;
		for (auto& iter : matMap)
		{
			if (iter.second.size() > 1)
			{
				// 相同材质对象数量大于1，进行合并，然后取代之前的vector列表
				auto batchedMeshRenderer = MergeMeshs(iter.second);
				iter.second.clear();
				iter.second.push_back(batchedMeshRenderer);
			}
		}

		// 把合批处理后的对象加入新列表
		for (auto& iter : matMap)
			for (auto renderer : iter.second)
				newList.push_back(renderer);

		// 参数引用传递，直接取代老列表
		batchRenderers = newList;
	}

	MeshRenderer* RenderQueue::MergeMeshs(RendererList& batchRenderers)
	{
		vector<Vertex> newVertices;
		vector<unsigned int> newIndices;

		unsigned int idxOffset = 0;
		for (auto renderer : batchRenderers)
		{
			auto mat_M = renderer->GetTransform()->GetModelMatrix();
			auto mat_M_IT = Math::Transpose(Math::Inverse(mat_M));
			for (auto mesh : renderer->meshes)
			{
				for (auto& vertex : mesh->vertices)
				{
					Vertex newVertex;
					newVertex.Position = mat_M * Vector4(vertex.Position, 1.0f);
					newVertex.Normal = Matrix3(mat_M_IT) * vertex.Normal;
					newVertex.Tangent = mat_M * Vector4(vertex.Tangent, 0.0f);
					newVertex.Bitangent = mat_M * Vector4(vertex.Bitangent, 0.0f);
					newVertex.TexCoords = vertex.TexCoords;
					newVertices.push_back(newVertex);
				}

				for (auto idx : mesh->indices)
					newIndices.push_back(idx + idxOffset);

				idxOffset += (unsigned int)mesh->vertices.size();
			}
		}

		auto newMesh = new Mesh(newVertices, newIndices);
		temporaryMeshPool.push_back(newMesh);
		auto newMeshRenderer = GetTemporaryRenderer();
		newMeshRenderer->matetrial = batchRenderers[0]->matetrial;
		newMeshRenderer->receiveShadow = batchRenderers[0]->receiveShadow;
		// 之前可能有数据，先清掉
		newMeshRenderer->meshes.clear();
		newMeshRenderer->meshes.push_back(newMesh);

		return newMeshRenderer;
	}

	MeshRenderer* RenderQueue::GetTemporaryRenderer()
	{
		MeshRenderer* tempRenderer = nullptr;
		if (temporaryRendererPool.size() > rendererPoolIdx)
		{
			tempRenderer = temporaryRendererPool[rendererPoolIdx];
		}
		else
		{
			auto gameObject = new GameObject();
			gameObject->AddComponent<Transform>();
			tempRenderer = gameObject->AddComponent<MeshRenderer>();
			temporaryRendererPool.push_back(tempRenderer);
		}
		rendererPoolIdx++;
		return tempRenderer;
	}
}