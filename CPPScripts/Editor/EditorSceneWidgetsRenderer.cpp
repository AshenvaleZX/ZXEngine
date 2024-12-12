#include "EditorSceneWidgetsRenderer.h"
#include "EditorDataManager.h"
#include "EditorCamera.h"
#include "../RenderAPI.h"
#include "../GameObject.h"
#include "../RenderEngineProperties.h"
#include "../Material.h"
#include "../MaterialData.h"
#include "../Resources.h"
#include "../RenderStateSetting.h"
#include "../ZShader.h"

namespace ZXEngine
{
	EditorSceneWidgetsRenderer* EditorSceneWidgetsRenderer::mInstance = nullptr;

	void EditorSceneWidgetsRenderer::Create()
	{
		mInstance = new EditorSceneWidgetsRenderer();
	}

	EditorSceneWidgetsRenderer* EditorSceneWidgetsRenderer::GetInstance()
	{
		return mInstance;
	}

	EditorSceneWidgetsRenderer::EditorSceneWidgetsRenderer()
	{
		mRenderStateSetting = new RenderStateSetting();

		PrefabStruct* prefab = Resources::LoadPrefab("Prefabs/TransWidgetPos.zxprefab", true);
		mTransWidgetPos = new GameObject(prefab);
		delete prefab;

		mTransWidgetRot = nullptr;
		mTransWidgetScale = nullptr;

		mDrawCommandID = RenderAPI::GetInstance()->AllocateDrawCommand(CommandType::ForwardRendering, ZX_CLEAR_FRAME_BUFFER_DEPTH_BIT);
	}

	EditorSceneWidgetsRenderer::~EditorSceneWidgetsRenderer()
	{
		delete mRenderStateSetting;

		delete mTransWidgetPos;
	}

	void EditorSceneWidgetsRenderer::Render()
	{
		auto dataMgr = EditorDataManager::GetInstance();

		if (dataMgr->isGameView || dataMgr->selectedGO == nullptr)
			return;

		auto renderAPI = RenderAPI::GetInstance();

		renderAPI->ClearFrameBuffer(ZX_CLEAR_FRAME_BUFFER_DEPTH_BIT);

		renderAPI->SetRenderState(mRenderStateSetting);

		RenderEngineProperties::GetInstance()->SetCameraProperties(EditorCamera::GetInstance()->mCamera);

		mTransWidgetPos->GetComponent<Transform>()->SetPosition(dataMgr->selectedGO->GetComponent<Transform>()->GetPosition());
		mTransWidgetPos->GetComponent<Transform>()->SetRotation(dataMgr->selectedGO->GetComponent<Transform>()->GetRotation());

		RenderObject(mTransWidgetPos);

		renderAPI->GenerateDrawCommand(mDrawCommandID);
	}

	void EditorSceneWidgetsRenderer::RenderObject(GameObject* obj)
	{
		auto engineProperties = RenderEngineProperties::GetInstance();

		auto renderer = obj->GetComponent<MeshRenderer>();
		if (renderer != nullptr)
		{
			auto material = renderer->mMatetrial;
			material->shader->Use();
			material->SetMaterialProperties();

			engineProperties->SetRendererProperties(renderer);

			material->SetEngineProperties();

			material->data->Use();

			renderer->Draw();
		}

		for (auto child : obj->children)
		{
			RenderObject(child);
		}
	}
}