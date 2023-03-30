#include "../RenderEngine.h"
#include "../ProjectSetting.h"
#include "../GlobalData.h"
#include "EditorGUIManager.h"
#include "EditorProjectPanel.h"
#include "EditorMainBarPanel.h"
#include "EditorInspectorPanel.h"
#include "EditorHierarchyPanel.h"
#include "EditorAssetPreviewer.h"
#include "EditorConsolePanel.h"

#ifdef ZX_API_VULKAN
#include "../RenderAPIVulkan.h"

static int g_MinImageCount = 2;
static ImGui_ImplVulkanH_Window g_MainWindowData;
static VkDescriptorPool g_DescriptorPool = VK_NULL_HANDLE;
static bool g_SwapChainRebuild = false;

static void check_vk_result(VkResult err)
{
	if (err == 0)
		return;
	fprintf(stderr, "[vulkan] Error: VkResult = %d\n", err);
	if (err < 0)
		abort();
}
#endif

namespace ZXEngine
{
	EditorGUIManager* EditorGUIManager::mInstance = nullptr;

	void EditorGUIManager::Create()
	{
		mInstance = new EditorGUIManager();
	}

	EditorGUIManager* EditorGUIManager::GetInstance()
	{
		return mInstance;
	}

	EditorGUIManager::EditorGUIManager()
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGui::StyleColorsLight();

#ifdef ZX_API_OPENGL
		const char* glsl_version = "#version 460";
		ImGui_ImplGlfw_InitForOpenGL(RenderEngine::GetInstance()->window, true);
		ImGui_ImplOpenGL3_Init(glsl_version);
#endif
#ifdef ZX_API_VULKAN
		auto renderAPI = reinterpret_cast<RenderAPIVulkan*>(RenderAPI::GetInstance());

		int w, h;
		glfwGetFramebufferSize(RenderEngine::GetInstance()->window, &w, &h);
		ImGui_ImplVulkanH_Window* wd = &g_MainWindowData;
		SetupVulkanWindow(wd, renderAPI->surface, w, h);

		VkDescriptorPoolSize pool_sizes[] =
		{
			{ VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
			{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
			{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
			{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
		};
		VkDescriptorPoolCreateInfo pool_info = {};
		pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
		pool_info.maxSets = 1000 * IM_ARRAYSIZE(pool_sizes);
		pool_info.poolSizeCount = (uint32_t)IM_ARRAYSIZE(pool_sizes);
		pool_info.pPoolSizes = pool_sizes;
		VkResult err = vkCreateDescriptorPool(renderAPI->device, &pool_info, VK_NULL_HANDLE, &g_DescriptorPool);
		check_vk_result(err);

		ImGui_ImplGlfw_InitForVulkan(RenderEngine::GetInstance()->window, true);
		ImGui_ImplVulkan_InitInfo init_info = {};
		init_info.Instance = renderAPI->vkInstance;
		init_info.PhysicalDevice = renderAPI->physicalDevice;
		init_info.Device = renderAPI->device;
		init_info.QueueFamily = renderAPI->queueFamilyIndices.graphics;
		init_info.Queue = renderAPI->graphicsQueue;
		init_info.PipelineCache = VK_NULL_HANDLE;
		init_info.DescriptorPool = g_DescriptorPool;
		init_info.Subpass = 0;
		init_info.MinImageCount = g_MinImageCount;
		init_info.ImageCount = wd->ImageCount;
		init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
		init_info.Allocator = VK_NULL_HANDLE;
		init_info.CheckVkResultFn = check_vk_result;
		ImGui_ImplVulkan_Init(&init_info, wd->RenderPass);
#endif

		allPanels.push_back(new EditorProjectPanel());
		allPanels.push_back(new EditorMainBarPanel());
		allPanels.push_back(new EditorInspectorPanel());
		allPanels.push_back(new EditorHierarchyPanel());
		allPanels.push_back(new EditorConsolePanel());
		assetPreviewer = new EditorAssetPreviewer();
	}

	EditorGUIManager::~EditorGUIManager()
	{
#ifdef ZX_API_OPENGL
		ImGui_ImplOpenGL3_Shutdown();
#endif
#ifdef ZX_API_VULKAN
		auto renderAPI = reinterpret_cast<RenderAPIVulkan*>(RenderAPI::GetInstance());
		VkResult err = vkDeviceWaitIdle(renderAPI->device);
		check_vk_result(err);
		ImGui_ImplVulkan_Shutdown();
#endif
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();

#ifdef ZX_API_VULKAN
		ImGui_ImplVulkanH_DestroyWindow(renderAPI->vkInstance, renderAPI->device, &g_MainWindowData, VK_NULL_HANDLE);
		vkDestroyDescriptorPool(renderAPI->device, g_DescriptorPool, VK_NULL_HANDLE);
#endif
	}

	void EditorGUIManager::BeginEditorRender()
	{
#ifdef ZX_API_OPENGL
		ImGui_ImplOpenGL3_NewFrame();
#endif
#ifdef ZX_API_VULKAN
		ImGui_ImplVulkan_NewFrame();
#endif
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	void EditorGUIManager::EditorRender()
	{
		if (assetPreviewer->Check())
			assetPreviewer->Draw();

		for (auto panel : allPanels)
		{
			panel->DrawPanel();
		}
		ImGui::Render();
	}

	void EditorGUIManager::EndEditorRender()
	{
#ifdef ZX_API_OPENGL
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
#endif
#ifdef ZX_API_VULKAN
		ImDrawData* draw_data = ImGui::GetDrawData();
		const bool is_minimized = (draw_data->DisplaySize.x <= 0.0f || draw_data->DisplaySize.y <= 0.0f);
		if (!is_minimized)
		{
			ImGui_ImplVulkanH_Window* wd = &g_MainWindowData;
			ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
			wd->ClearValue.color.float32[0] = clear_color.x * clear_color.w;
			wd->ClearValue.color.float32[1] = clear_color.y * clear_color.w;
			wd->ClearValue.color.float32[2] = clear_color.z * clear_color.w;
			wd->ClearValue.color.float32[3] = clear_color.w;
			FrameRender(wd, draw_data);
			FramePresent(wd);
		}
#endif
	}

	void EditorGUIManager::ResetPanels()
	{
		for (auto panel : allPanels)
			panel->ResetPanel();
	}

#ifdef ZX_API_VULKAN
	void EditorGUIManager::SetupVulkanWindow(ImGui_ImplVulkanH_Window* wd, VkSurfaceKHR surface, int width, int height)
	{
		auto renderAPI = reinterpret_cast<RenderAPIVulkan*>(RenderAPI::GetInstance());
		wd->Surface = surface;

		// Check for WSI support
		VkBool32 res;
		vkGetPhysicalDeviceSurfaceSupportKHR(renderAPI->physicalDevice, renderAPI->queueFamilyIndices.graphics, wd->Surface, &res);
		if (res != VK_TRUE)
		{
			fprintf(stderr, "Error no WSI support on physical device 0\n");
			exit(-1);
		}

		// Select Surface Format
		const VkFormat requestSurfaceImageFormat[] = { VK_FORMAT_B8G8R8A8_UNORM, VK_FORMAT_R8G8B8A8_UNORM, VK_FORMAT_B8G8R8_UNORM, VK_FORMAT_R8G8B8_UNORM };
		const VkColorSpaceKHR requestSurfaceColorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;
		wd->SurfaceFormat = ImGui_ImplVulkanH_SelectSurfaceFormat(renderAPI->physicalDevice, wd->Surface, requestSurfaceImageFormat, (size_t)IM_ARRAYSIZE(requestSurfaceImageFormat), requestSurfaceColorSpace);

		// Select Present Mode
#ifdef IMGUI_UNLIMITED_FRAME_RATE
		VkPresentModeKHR present_modes[] = { VK_PRESENT_MODE_MAILBOX_KHR, VK_PRESENT_MODE_IMMEDIATE_KHR, VK_PRESENT_MODE_FIFO_KHR };
#else
		VkPresentModeKHR present_modes[] = { VK_PRESENT_MODE_FIFO_KHR };
#endif
		wd->PresentMode = ImGui_ImplVulkanH_SelectPresentMode(renderAPI->physicalDevice, wd->Surface, &present_modes[0], IM_ARRAYSIZE(present_modes));

		// Create SwapChain, RenderPass, Framebuffer, etc.
		IM_ASSERT(g_MinImageCount >= 2);
		ImGui_ImplVulkanH_CreateOrResizeWindow(renderAPI->vkInstance, renderAPI->physicalDevice, renderAPI->device, wd, renderAPI->queueFamilyIndices.graphics, VK_NULL_HANDLE, width, height, g_MinImageCount);
	}

	void EditorGUIManager::FrameRender(ImGui_ImplVulkanH_Window* wd, ImDrawData* draw_data)
	{
		auto renderAPI = reinterpret_cast<RenderAPIVulkan*>(RenderAPI::GetInstance());

		VkResult err;
		VkSemaphore image_acquired_semaphore = wd->FrameSemaphores[wd->SemaphoreIndex].ImageAcquiredSemaphore;
		VkSemaphore render_complete_semaphore = wd->FrameSemaphores[wd->SemaphoreIndex].RenderCompleteSemaphore;
		err = vkAcquireNextImageKHR(renderAPI->device, wd->Swapchain, UINT64_MAX, image_acquired_semaphore, VK_NULL_HANDLE, &wd->FrameIndex);
		if (err == VK_ERROR_OUT_OF_DATE_KHR || err == VK_SUBOPTIMAL_KHR)
		{
			g_SwapChainRebuild = true;
			return;
		}
		check_vk_result(err);

		ImGui_ImplVulkanH_Frame* fd = &wd->Frames[wd->FrameIndex];
		{
			err = vkWaitForFences(renderAPI->device, 1, &fd->Fence, VK_TRUE, UINT64_MAX);    // wait indefinitely instead of periodically checking
			check_vk_result(err);

			err = vkResetFences(renderAPI->device, 1, &fd->Fence);
			check_vk_result(err);
		}
		{
			err = vkResetCommandPool(renderAPI->device, fd->CommandPool, 0);
			check_vk_result(err);
			VkCommandBufferBeginInfo info = {};
			info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
			err = vkBeginCommandBuffer(fd->CommandBuffer, &info);
			check_vk_result(err);
		}
		{
			VkRenderPassBeginInfo info = {};
			info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			info.renderPass = wd->RenderPass;
			info.framebuffer = fd->Framebuffer;
			info.renderArea.extent.width = wd->Width;
			info.renderArea.extent.height = wd->Height;
			info.clearValueCount = 1;
			info.pClearValues = &wd->ClearValue;
			vkCmdBeginRenderPass(fd->CommandBuffer, &info, VK_SUBPASS_CONTENTS_INLINE);
		}

		// Record dear imgui primitives into command buffer
		ImGui_ImplVulkan_RenderDrawData(draw_data, fd->CommandBuffer);

		// Submit command buffer
		vkCmdEndRenderPass(fd->CommandBuffer);
		{
			VkPipelineStageFlags wait_stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			VkSubmitInfo info = {};
			info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			info.waitSemaphoreCount = 1;
			info.pWaitSemaphores = &image_acquired_semaphore;
			info.pWaitDstStageMask = &wait_stage;
			info.commandBufferCount = 1;
			info.pCommandBuffers = &fd->CommandBuffer;
			info.signalSemaphoreCount = 1;
			info.pSignalSemaphores = &render_complete_semaphore;

			err = vkEndCommandBuffer(fd->CommandBuffer);
			check_vk_result(err);
			err = vkQueueSubmit(renderAPI->graphicsQueue, 1, &info, fd->Fence);
			check_vk_result(err);
		}
	}

	void EditorGUIManager::FramePresent(ImGui_ImplVulkanH_Window* wd)
	{
		auto renderAPI = reinterpret_cast<RenderAPIVulkan*>(RenderAPI::GetInstance());

		if (g_SwapChainRebuild)
			return;
		VkSemaphore render_complete_semaphore = wd->FrameSemaphores[wd->SemaphoreIndex].RenderCompleteSemaphore;
		VkPresentInfoKHR info = {};
		info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		info.waitSemaphoreCount = 1;
		info.pWaitSemaphores = &render_complete_semaphore;
		info.swapchainCount = 1;
		info.pSwapchains = &wd->Swapchain;
		info.pImageIndices = &wd->FrameIndex;
		VkResult err = vkQueuePresentKHR(renderAPI->graphicsQueue, &info);
		if (err == VK_ERROR_OUT_OF_DATE_KHR || err == VK_SUBOPTIMAL_KHR)
		{
			g_SwapChainRebuild = true;
			return;
		}
		check_vk_result(err);
		wd->SemaphoreIndex = (wd->SemaphoreIndex + 1) % wd->ImageCount; // Now we can use the next set of semaphores
	}
#endif
}