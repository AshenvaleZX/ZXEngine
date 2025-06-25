#include "EditorGUIManagerMetal.h"
#include "EditorAssetPreviewer.h"
#include "EditorDialogBoxManager.h"
#include "../FBOManager.h"
#include "../RenderAPIMetal.h"
#include "../Window/WindowManager.h"
#include "../External/ImGui/imgui_impl_glfw.h"
#include "../External/ImGui/imgui_impl_metal.h"

namespace ZXEngine
{
	EditorGUIManagerMetal::EditorGUIManagerMetal()
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGui::StyleColorsLight();

		InitForMetal();
	}

	EditorGUIManagerMetal::~EditorGUIManagerMetal()
	{
		ImGui_ImplMetal_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	void EditorGUIManagerMetal::BeginEditorRender()
	{
		RenderAPIMetal* renderAPI = static_cast<RenderAPIMetal*>(RenderAPI::GetInstance());
		mRenderPassDescriptor->colorAttachments()->object(0)->setTexture(renderAPI->mDrawable->texture());

		ImGui_ImplMetal_NewFrame(mRenderPassDescriptor);
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	void EditorGUIManagerMetal::EditorRender()
	{
		if (assetPreviewer->Check())
			assetPreviewer->Draw();

		FBOManager::GetInstance()->SwitchFBO(ScreenBuffer);

		for (auto panel : mAllPanels)
		{
			panel->DrawPanel();
		}

		EditorDialogBoxManager::GetInstance()->Draw();

		ImGui::Render();
	}

	void EditorGUIManagerMetal::EndEditorRender()
	{
		RenderAPIMetal* renderAPI = static_cast<RenderAPIMetal*>(RenderAPI::GetInstance());

		MTL::CommandBuffer* commandBuffer = renderAPI->mCommandQueue->commandBuffer();
		MTL::RenderCommandEncoder* renderEncoder = commandBuffer->renderCommandEncoder(mRenderPassDescriptor);

		ImGui_ImplMetal_RenderDrawData(ImGui::GetDrawData(), commandBuffer, renderEncoder);

		renderEncoder->endEncoding();

		commandBuffer->presentDrawable(renderAPI->mDrawable);
		commandBuffer->addCompletedHandler([=](MTL::CommandBuffer* cmd)
		{
			dispatch_semaphore_signal(renderAPI->mSemaphore);
		});
		commandBuffer->commit();
	}

	void EditorGUIManagerMetal::OnWindowSizeChange()
	{

	}

	void EditorGUIManagerMetal::InitForMetal()
	{
		ImGui_ImplGlfw_InitForOpenGL(static_cast<GLFWwindow*>(WindowManager::GetInstance()->GetWindow()), true);
		ImGui_ImplMetal_Init(static_cast<RenderAPIMetal*>(RenderAPI::GetInstance())->mDevice);

		mRenderPassDescriptor = MTL::RenderPassDescriptor::alloc()->init();
		mRenderPassDescriptor->colorAttachments()->object(0)->setLoadAction(MTL::LoadActionLoad);
		mRenderPassDescriptor->colorAttachments()->object(0)->setStoreAction(MTL::StoreActionStore);
	}
}