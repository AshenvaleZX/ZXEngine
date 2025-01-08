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
#include "../GlobalData.h"

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

		PrefabStruct* prefab = Resources::LoadPrefab("Prefabs/WorldTransWidget.zxprefab", true);
		mWorldTransWidget = new GameObject(prefab);
		delete prefab;

		InitWorldTransWidgetCamera();

		mDrawCommandID = RenderAPI::GetInstance()->AllocateDrawCommand(CommandType::ForwardRendering, ZX_CLEAR_FRAME_BUFFER_DEPTH_BIT);
		mDrawWorldTransCommandID = RenderAPI::GetInstance()->AllocateDrawCommand(CommandType::ForwardRendering, ZX_CLEAR_FRAME_BUFFER_DEPTH_BIT);
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

		delete mWorldTransWidget;
		delete mWorldTransWidgetCamera;
	}

	void EditorSceneWidgetsRenderer::Render()
	{
		auto dataMgr = EditorDataManager::GetInstance();

		if (dataMgr->isGameView)
			return;

		auto renderAPI = RenderAPI::GetInstance();

		if (dataMgr->selectedGO == nullptr)
		{
			renderAPI->ClearFrameBuffer(ZX_CLEAR_FRAME_BUFFER_DEPTH_BIT);

			DrawWorldTransWidget();
		}
		else
		{
			// Selected Object Outline
			FBOManager::GetInstance()->SwitchFBO("Silhouette");

			renderAPI->ClearFrameBuffer(ZX_CLEAR_FRAME_BUFFER_COLOR_BIT);

			auto editorCamera = EditorCamera::GetInstance();
			RenderEngineProperties::GetInstance()->SetCameraProperties(editorCamera->mCamera);

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

			auto widget = dataMgr->GetTransWidget();

			auto goTrans = dataMgr->selectedGO->GetComponent<Transform>();
			auto widgetTrans = widget->GetComponent<Transform>();

			Vector3 camPos = editorCamera->mCameraTrans->GetPosition();
			Vector3 dir = goTrans->GetPosition() - camPos;
			dir.Normalize();

			widgetTrans->SetPosition(camPos + dir * WidgetDis);
			widgetTrans->SetRotation(goTrans->GetRotation());

			if (dataMgr->mCurTransType == TransformType::Rotation)
			{
				for (auto& iter : dataMgr->mTransRotWidgetTurnplates)
				{
					iter.second->GetComponent<MeshRenderer>()->mMatetrial->SetScalar("_Angle", iter.first == editorCamera->GetCurAxis() ? editorCamera->GetRotationRadian() : 0.0f);
				}
			}
			RenderWidget(widget);

			renderAPI->GenerateDrawCommand(mDrawCommandID);

			// World Axis Widget
			DrawWorldTransWidget();
		}
	}

	void EditorSceneWidgetsRenderer::UpdateWidgetColor(bool isSelect)
	{
		AxisType axisType = EditorCamera::GetInstance()->GetCurAxis();
		if (axisType == AxisType::None)
			return;

		auto dataMgr = EditorDataManager::GetInstance();
		if (dataMgr->mCurTransType == TransformType::Rotation)
		{
			for (auto& iter : dataMgr->mTransRotWidgetTurnplates)
			{
				Vector4 color;

				if (isSelect)
				{
					if (iter.first == axisType)
					{
						color = Vector4(1.0f, 1.0f, 0.0f, 1.0f);
					}
					else
					{
						switch (iter.first)
						{
						case AxisType::X:
							color = Vector4(1.0f, 0.0f, 0.0f, 0.5f);
							break;
						case AxisType::Y:
							color = Vector4(0.0f, 1.0f, 0.0f, 0.5f);
							break;
						case AxisType::Z:
							color = Vector4(0.0f, 0.0f, 1.0f, 0.5f);
							break;
						default:
							break;
						}
					}
				}
				else
				{
					switch (iter.first)
					{
					case AxisType::X:
						color = Vector4(1.0f, 0.0f, 0.0f, 0.9f);
						break;
					case AxisType::Y:
						color = Vector4(0.0f, 1.0f, 0.0f, 0.9f);
						break;
					case AxisType::Z:
						color = Vector4(0.0f, 0.0f, 1.0f, 0.9f);
						break;
					default:
						break;
					}
				}

				iter.second->GetComponent<MeshRenderer>()->mMatetrial->SetVector("_Color", color, true);
			}
		}
		else
		{
			WidgetOrientationMap& axisGOs = dataMgr->mCurTransType == TransformType::Position ? dataMgr->mTransPosWidgetOrientations : dataMgr->mTransScaleWidgetOrientations;

			for (auto& iter : axisGOs)
			{
				Vector4 color;

				if (isSelect)
				{
					if (iter.first == axisType)
						color = Vector4(1.0f, 1.0f, 0.0f, 1.0f);
					else
						color = Vector4(0.8f, 0.8f, 0.8f, 1.0f);
				}
				else
				{
					switch (iter.first)
					{
					case AxisType::X:
						color = Vector4(1.0f, 0.0f, 0.0f, 1.0f);
						break;
					case AxisType::Y:
						color = Vector4(0.0f, 1.0f, 0.0f, 1.0f);
						break;
					case AxisType::Z:
						color = Vector4(0.0f, 0.0f, 1.0f, 1.0f);
						break;
					default:
						break;
					}
				}

				iter.second.first->GetComponent<MeshRenderer>()->mMatetrial->SetVector("_Color", color, true);
				iter.second.second->GetComponent<MeshRenderer>()->mMatetrial->SetVector("_Color", color, true);
			}
		}
	}

	void EditorSceneWidgetsRenderer::InitWorldTransWidgetCamera()
	{
		mWorldTransWidgetCamera = new GameObject();

		auto transform = mWorldTransWidgetCamera->AddComponent<Transform>();
		transform->SetPosition(Vector3(0.0f, 0.0f, -8.0f));

		auto camera = mWorldTransWidgetCamera->AddComponent<Camera>();
		camera->mAspect = 1.0f;
		camera->mCameraType = CameraType::EditorCamera;
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

	void EditorSceneWidgetsRenderer::DrawWorldTransWidget()
	{
		auto renderAPI = RenderAPI::GetInstance();

		mRenderStateSetting->depthTest = true;
		mRenderStateSetting->depthWrite = true;
		renderAPI->SetRenderState(mRenderStateSetting);

		const static uint32_t Edge = 10;
		const static uint32_t Size = 100;

#ifdef ZX_API_OPENGL
		renderAPI->SetViewPort(Size, Size, GlobalData::srcWidth - Size - Edge, GlobalData::srcHeight - Size - Edge);
#else
		renderAPI->SetViewPort(Size, Size, GlobalData::srcWidth - Size - Edge, Edge);
#endif

		RenderEngineProperties::GetInstance()->SetCameraProperties(mWorldTransWidgetCamera->GetComponent<Camera>());

		mWorldTransWidget->GetComponent<Transform>()->SetRotation(EditorCamera::GetInstance()->mCameraTrans->GetRotation().GetInverse());

		RenderWidget(mWorldTransWidget);

		renderAPI->GenerateDrawCommand(mDrawWorldTransCommandID);
	}
}