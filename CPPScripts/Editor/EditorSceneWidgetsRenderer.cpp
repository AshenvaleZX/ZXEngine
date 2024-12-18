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
#include "../FBOManager.h"
#include "../GeometryGenerator.h"
#include "../StaticMesh.h"

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
		mRenderStateSetting->faceCull = false;

		mDrawCommandID = RenderAPI::GetInstance()->AllocateDrawCommand(CommandType::ForwardRendering, ZX_CLEAR_FRAME_BUFFER_DEPTH_BIT);
		mDrawSilhouetteCommandID = RenderAPI::GetInstance()->AllocateDrawCommand(CommandType::ForwardRendering, ZX_CLEAR_FRAME_BUFFER_COLOR_BIT);

		FBOManager::GetInstance()->CreateFBO("Silhouette", FrameBufferType::Color);

		mScreenQuad = GeometryGenerator::CreateScreenQuad();

		mSilhouetteShader = new Shader(Resources::GetAssetFullPath("Shaders/Silhouette.zxshader", true), FrameBufferType::Color);
		mSilhouetteOutlineShader = new Shader(Resources::GetAssetFullPath("Shaders/SilhouetteOutline.zxshader", true), FrameBufferType::Normal);
		mSilhouetteOutlineMaterial = new Material(mSilhouetteOutlineShader);

		EditorDataManager::GetInstance()->InitWidgets();
	}

	EditorSceneWidgetsRenderer::~EditorSceneWidgetsRenderer()
	{
		delete mRenderStateSetting;
		delete mSilhouetteShader;
		delete mSilhouetteOutlineShader;
		delete mSilhouetteOutlineMaterial;

		for (auto material : mSilhouetteMaterials)
		{
			delete material;
		}
	}

	void EditorSceneWidgetsRenderer::Render()
	{
		auto dataMgr = EditorDataManager::GetInstance();

		if (dataMgr->isGameView || dataMgr->selectedGO == nullptr)
			return;

		auto renderAPI = RenderAPI::GetInstance();

		// Selected Object Outline
		FBOManager::GetInstance()->SwitchFBO("Silhouette");

		renderAPI->ClearFrameBuffer(ZX_CLEAR_FRAME_BUFFER_COLOR_BIT);

		size_t idx = 0;
		DrawObjectSilhouette(dataMgr->selectedGO, idx);

		RenderAPI::GetInstance()->GenerateDrawCommand(mDrawSilhouetteCommandID);

		FBOManager::GetInstance()->SwitchFBO("GameView");

		renderAPI->ClearFrameBuffer(ZX_CLEAR_FRAME_BUFFER_DEPTH_BIT);

		mRenderStateSetting->depthTest = false;
		mRenderStateSetting->depthWrite = false;
		renderAPI->SetRenderState(mRenderStateSetting);

		DrawSilhouetteOutline();

		// Widget
		mRenderStateSetting->depthTest = dataMgr->mCurTransType != TransformType::Rotation;
		mRenderStateSetting->depthWrite = dataMgr->mCurTransType != TransformType::Rotation;
		renderAPI->SetRenderState(mRenderStateSetting);

		RenderEngineProperties::GetInstance()->SetCameraProperties(EditorCamera::GetInstance()->mCamera);

		auto widget = dataMgr->GetTransWidget();

		auto goTrans = dataMgr->selectedGO->GetComponent<Transform>();
		auto widgetTrans = widget->GetComponent<Transform>();

		Vector3 camPos = EditorCamera::GetInstance()->mCameraTrans->GetPosition();
		Vector3 dir = goTrans->GetPosition() - camPos;
		dir.Normalize();

		widgetTrans->SetPosition(camPos + dir * WidgetDis);
		widgetTrans->SetRotation(goTrans->GetRotation());

		RenderWidget(widget);

		renderAPI->GenerateDrawCommand(mDrawCommandID);
	}

	void EditorSceneWidgetsRenderer::RenderWidget(GameObject* obj)
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
			RenderWidget(child);
		}
	}

	void EditorSceneWidgetsRenderer::DrawObjectSilhouette(GameObject* obj, size_t& idx)
	{
		auto renderer = obj->GetComponent<MeshRenderer>();
		if (renderer != nullptr)
		{
			if (mSilhouetteMaterials.size() < idx + 1)
			{
				mSilhouetteMaterials.push_back(new Material(mSilhouetteShader));
			}
			auto material = mSilhouetteMaterials[idx];

			material->shader->Use();
			material->data->Use();

			RenderEngineProperties::GetInstance()->SetRendererProperties(renderer);

			material->SetEngineProperties();

			renderer->Draw();

			idx++;
		}

		for (auto child : obj->children)
		{
			DrawObjectSilhouette(child, idx);
		}
	}

	void EditorSceneWidgetsRenderer::DrawSilhouetteOutline()
	{
		mSilhouetteOutlineMaterial->Use();
		mSilhouetteOutlineMaterial->SetTexture("_RenderTexture", FBOManager::GetInstance()->GetFBO("Silhouette")->ColorBuffer, 0, false, true);
		RenderAPI::GetInstance()->Draw(mScreenQuad->VAO);
	}
}