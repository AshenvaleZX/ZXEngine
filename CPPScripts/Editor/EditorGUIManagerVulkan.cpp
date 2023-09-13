#include "EditorGUIManagerVulkan.h"
#include "EditorProjectPanel.h"
#include "EditorMainBarPanel.h"
#include "EditorHierarchyPanel.h"
#include "EditorInspectorPanel.h"
#include "EditorConsolePanel.h"
#include "EditorAssetPreviewer.h"
#include "EditorDialogBoxManager.h"
#include "../RenderAPIVulkan.h"
#include "../Window/WindowManager.h"
#include "../External/ImGui/imgui_impl_glfw.h"
#include "../External/ImGui/imgui_impl_vulkan.h"

// Vulkan的ImGui接入比较麻烦，参考教程: https://frguthmann.github.io/posts/vulkan_imgui/
static int g_MinImageCount = 2;
static VkDescriptorPool g_DescriptorPool = VK_NULL_HANDLE;
static VkRenderPass g_RenderPass = VK_NULL_HANDLE;
static VkCommandPool g_CommandPool = VK_NULL_HANDLE;
static vector<VkCommandBuffer> g_CommandBuffers;
static vector<VkFramebuffer> g_FrameBuffers;

static void check_vk_result(VkResult err)
{
	if (err == 0)
		return;
	fprintf(stderr, "ImGui Error: VkResult = %d\n", err);
	if (err < 0)
		abort();
}

namespace ZXEngine
{
	EditorGUIManagerVulkan::EditorGUIManagerVulkan()
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGui::StyleColorsLight();

		InitForVulkan();
	}

	EditorGUIManagerVulkan::~EditorGUIManagerVulkan()
	{
		auto renderAPI = reinterpret_cast<RenderAPIVulkan*>(RenderAPI::GetInstance());
		VkResult err = vkDeviceWaitIdle(renderAPI->device);
		check_vk_result(err);
		ImGui_ImplVulkan_Shutdown();

		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();

		vkDestroyDescriptorPool(renderAPI->device, g_DescriptorPool, VK_NULL_HANDLE);
	}

	void EditorGUIManagerVulkan::Init()
	{
		// Inspector的绘制要放在Hierarchy和Project后面，因为这两个面板会决定Inspector的内容
		allPanels.push_back(new EditorProjectPanel());
		allPanels.push_back(new EditorMainBarPanel());
		allPanels.push_back(new EditorHierarchyPanel());
		allPanels.push_back(new EditorInspectorPanel());
		allPanels.push_back(new EditorConsolePanel());
		assetPreviewer = new EditorAssetPreviewer();
	}

	void EditorGUIManagerVulkan::BeginEditorRender()
	{
		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	void EditorGUIManagerVulkan::EditorRender()
	{
		if (assetPreviewer->Check())
			assetPreviewer->Draw();

		for (auto panel : allPanels)
		{
			panel->DrawPanel();
		}

		EditorDialogBoxManager::GetInstance()->Draw();

		ImGui::Render();
	}

	void EditorGUIManagerVulkan::EndEditorRender()
	{
		FrameRender();
	}

	void EditorGUIManagerVulkan::ResetPanels()
	{
		for (auto panel : allPanels)
			panel->ResetPanel();
	}

	void EditorGUIManagerVulkan::OnWindowSizeChange()
	{
		RecreateFrameBuffers();
	}

	void EditorGUIManagerVulkan::InitForVulkan()
	{
		auto renderAPI = reinterpret_cast<RenderAPIVulkan*>(RenderAPI::GetInstance());

		// 创建一个专用的DescriptorPool
		{
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
		}

		// 创建一个专用的RenderPass
		{
			VkAttachmentDescription colorAttachment = {};
			colorAttachment.format = renderAPI->swapChainImageFormat;
			colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
			colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
			colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
			colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			colorAttachment.initialLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
			colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

			VkAttachmentReference colorAttachmentRef = {};
			colorAttachmentRef.attachment = 0;
			colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

			VkSubpassDescription subpass = {};
			subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
			subpass.colorAttachmentCount = 1;
			subpass.pColorAttachments = &colorAttachmentRef;

			VkSubpassDependency dependency = {};
			dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
			dependency.dstSubpass = 0;
			dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			dependency.srcAccessMask = 0;  // or VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

			VkRenderPassCreateInfo info = {};
			info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
			info.attachmentCount = 1;
			info.pAttachments = &colorAttachment;
			info.subpassCount = 1;
			info.pSubpasses = &subpass;
			info.dependencyCount = 1;
			info.pDependencies = &dependency;
			if (vkCreateRenderPass(renderAPI->device, &info, VK_NULL_HANDLE, &g_RenderPass) != VK_SUCCESS)
				throw std::runtime_error("Could not create Dear ImGui's render pass");
		}

		// 创建专用的FrameBuffer
		{
			g_FrameBuffers.resize(renderAPI->swapChainImageViews.size());
			for (size_t i = 0; i < renderAPI->swapChainImageViews.size(); i++)
			{
				VkFramebufferCreateInfo info = {};
				info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
				info.renderPass = g_RenderPass;
				info.pAttachments = &renderAPI->swapChainImageViews[i];
				info.attachmentCount = 1;
				info.width = renderAPI->swapChainExtent.width;
				info.height = renderAPI->swapChainExtent.height;
				info.layers = 1;

				VkResult err = vkCreateFramebuffer(renderAPI->device, &info, VK_NULL_HANDLE, &g_FrameBuffers[i]);
				check_vk_result(err);
			}
		}

		// 创建一个专用的CommandPool
		{
			VkCommandPoolCreateInfo commandPoolCreateInfo = {};
			commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
			commandPoolCreateInfo.queueFamilyIndex = renderAPI->queueFamilyIndices.graphics;
			commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

			if (vkCreateCommandPool(renderAPI->device, &commandPoolCreateInfo, VK_NULL_HANDLE, &g_CommandPool) != VK_SUCCESS)
				throw std::runtime_error("Could not create graphics command pool");
		}

		// 创建专用的CommandBuffer
		{
			g_CommandBuffers.resize(MAX_FRAMES_IN_FLIGHT);

			VkCommandBufferAllocateInfo allocInfo = {};
			allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
			allocInfo.commandPool = g_CommandPool;
			allocInfo.commandBufferCount = MAX_FRAMES_IN_FLIGHT;

			if (vkAllocateCommandBuffers(renderAPI->device, &allocInfo, g_CommandBuffers.data()) != VK_SUCCESS)
				throw std::runtime_error("failed to allocate command buffers!");
		}

		// 在调用ImGui_ImplVulkan_Init初始化基于Vulkan的ImGui之前，先调用这个接口，给ImGui一个可以加载Vulkan函数的函数
		// 其实本来可以不需要这一步的，因为ImGui的绘制只会用到Vulkan的Core函数，而Vulkan的Core函数不需要我们手动加载
		// 但是如果我们要使用Vulkan光追，就要启用相关扩展，而Vulkan扩展的函数虽然在vulkan.h的头文件里，但是这些函数并没有直接加载好
		// 需要我们自己去加载函数(绑定函数指针)，所以因此间接影响了基于Vulkan的ImGui
		// 虽然基于Vulkan的ImGui渲染并不需要任何扩展，但是因为我们要自己加载所有Vulkan函数了，就也需要给ImGui传递一个函数，让ImGui去找Vulkan函数地址
		ImGui_ImplVulkan_LoadFunctions([](const char* name, void*) { return vkGetInstanceProcAddr(volkGetLoadedInstance(), name); });

		// 初始化Vulkan版本的ImGUI
		ImGui_ImplGlfw_InitForVulkan(static_cast<GLFWwindow*>(WindowManager::GetInstance()->GetWindow()), true);
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
		init_info.ImageCount = (uint32_t)renderAPI->swapChainImages.size();
		init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
		init_info.Allocator = VK_NULL_HANDLE;
		init_info.CheckVkResultFn = check_vk_result;
		ImGui_ImplVulkan_Init(&init_info, g_RenderPass);

		// 初始化字体
		renderAPI->ImmediatelyExecute([=](VkCommandBuffer cmd) { ImGui_ImplVulkan_CreateFontsTexture(cmd); });
	}

	void EditorGUIManagerVulkan::FrameRender()
	{
		auto renderAPI = reinterpret_cast<RenderAPIVulkan*>(RenderAPI::GetInstance());

		auto commandBuffer = g_CommandBuffers[renderAPI->currentFrame];
		vkResetCommandBuffer(commandBuffer, 0);

		VkCommandBufferBeginInfo commandInfo = {};
		commandInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		commandInfo.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		VkResult err = vkBeginCommandBuffer(commandBuffer, &commandInfo);
		check_vk_result(err);

		VkRenderPassBeginInfo renderPassinfo = {};
		renderPassinfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassinfo.renderPass = g_RenderPass;
		renderPassinfo.framebuffer = g_FrameBuffers[renderAPI->curPresentImageIdx];
		renderPassinfo.renderArea.extent.width = renderAPI->swapChainExtent.width;
		renderPassinfo.renderArea.extent.height = renderAPI->swapChainExtent.height;
		renderPassinfo.clearValueCount = 0;
		vkCmdBeginRenderPass(commandBuffer, &renderPassinfo, VK_SUBPASS_CONTENTS_INLINE);

		// Record Imgui Draw Data and draw funcs into command buffer
		ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffer);

		vkCmdEndRenderPass(commandBuffer);
		err = vkEndCommandBuffer(commandBuffer);
		check_vk_result(err);

		// 提交CommandBuffer
		vector<VkPipelineStageFlags> waitStages = {};
		waitStages.resize(renderAPI->curWaitSemaphores.size(), VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT);
		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.pCommandBuffers = &commandBuffer;
		submitInfo.commandBufferCount = 1;
		submitInfo.pWaitSemaphores = renderAPI->curWaitSemaphores.data();
		submitInfo.pWaitDstStageMask = waitStages.data();
		submitInfo.waitSemaphoreCount = static_cast<uint32_t>(renderAPI->curWaitSemaphores.size());
		// 这是固定在最后提交的，所以没有要等这个Command的地方，也不用激发信号量
		submitInfo.signalSemaphoreCount = 0;

		VkFence fence = renderAPI->inFlightFences[renderAPI->currentFrame];
		if (vkQueueSubmit(renderAPI->graphicsQueue, 1, &submitInfo, fence) != VK_SUCCESS)
			throw std::runtime_error("failed to submit draw command buffer!");

		renderAPI->curWaitSemaphores.clear();
	}

	void EditorGUIManagerVulkan::RecreateFrameBuffers()
	{
		auto renderAPI = reinterpret_cast<RenderAPIVulkan*>(RenderAPI::GetInstance());

		for (auto iter : g_FrameBuffers)
			vkDestroyFramebuffer(renderAPI->device, iter, VK_NULL_HANDLE);
		g_FrameBuffers.clear();

		ImGui_ImplVulkan_SetMinImageCount(static_cast<uint32_t>(renderAPI->swapChainImageViews.size()));

		g_FrameBuffers.resize(renderAPI->swapChainImageViews.size());
		for (size_t i = 0; i < renderAPI->swapChainImageViews.size(); i++)
		{
			VkFramebufferCreateInfo info = {};
			info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			info.renderPass = g_RenderPass;
			info.pAttachments = &renderAPI->swapChainImageViews[i];
			info.attachmentCount = 1;
			info.width = renderAPI->swapChainExtent.width;
			info.height = renderAPI->swapChainExtent.height;
			info.layers = 1;

			VkResult err = vkCreateFramebuffer(renderAPI->device, &info, VK_NULL_HANDLE, &g_FrameBuffers[i]);
			check_vk_result(err);
		}
	}
}