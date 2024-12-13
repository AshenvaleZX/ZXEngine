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
	constexpr float WidgetDis = 100.0f;

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
		mDrawCommandID = RenderAPI::GetInstance()->AllocateDrawCommand(CommandType::ForwardRendering, ZX_CLEAR_FRAME_BUFFER_DEPTH_BIT);

		EditorDataManager::GetInstance()->InitWidgets();
	}

	EditorSceneWidgetsRenderer::~EditorSceneWidgetsRenderer()
	{
		delete mRenderStateSetting;
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

		auto widget = dataMgr->mTransPosWidget;

		auto goTrans = dataMgr->selectedGO->GetComponent<Transform>();
		auto widgetTrans = widget->GetComponent<Transform>();

		Vector3 camPos = EditorCamera::GetInstance()->mCameraTrans->GetPosition();
		Vector3 dir = goTrans->GetPosition() - camPos;
		dir.Normalize();

		widgetTrans->SetPosition(camPos + dir * WidgetDis);
		widgetTrans->SetRotation(goTrans->GetRotation());

		RenderObject(widget);

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