#include "RenderAPIVulkan.h"
#include "GlobalData.h"
#include <stb_image.h>
#include "ShaderParser.h"
#include "Resources.h"
#include "Texture.h"
#include "ZShader.h"
#include "Material.h"
#include "MaterialData.h"
#include "ProjectSetting.h"
#include "FBOManager.h"
#include "Window/WindowManager.h"
#ifdef ZX_EDITOR
#include "Editor/EditorGUIManager.h"
#include "Editor/ImGuiTextureManager.h"
#endif


// VMA的官方文档里说需要在一个CPP文件里定义这个宏定义，否则可能会有异常
// 见:https://gpuopen-librariesandsdks.github.io/VulkanMemoryAllocator/html/quick_start.html#quick_start_project_setup
#define VMA_IMPLEMENTATION
#include "vk_mem_alloc.h"

namespace ZXEngine
{
    map<BlendFactor, VkBlendFactor> vkBlendFactorMap =
    {
        { BlendFactor::ZERO,           VK_BLEND_FACTOR_ZERO           }, { BlendFactor::ONE,                      VK_BLEND_FACTOR_ONE                      },
        { BlendFactor::SRC_COLOR,      VK_BLEND_FACTOR_SRC_COLOR      }, { BlendFactor::ONE_MINUS_SRC_COLOR,      VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR      },
        { BlendFactor::DST_COLOR,      VK_BLEND_FACTOR_DST_COLOR      }, { BlendFactor::ONE_MINUS_DST_COLOR,      VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR      },
        { BlendFactor::SRC_ALPHA,      VK_BLEND_FACTOR_SRC_ALPHA      }, { BlendFactor::ONE_MINUS_SRC_ALPHA,      VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA      },
        { BlendFactor::DST_ALPHA,      VK_BLEND_FACTOR_DST_ALPHA      }, { BlendFactor::ONE_MINUS_DST_ALPHA,      VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA      },
        { BlendFactor::CONSTANT_COLOR, VK_BLEND_FACTOR_CONSTANT_COLOR }, { BlendFactor::ONE_MINUS_CONSTANT_COLOR, VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR },
        { BlendFactor::CONSTANT_ALPHA, VK_BLEND_FACTOR_CONSTANT_ALPHA }, { BlendFactor::ONE_MINUS_CONSTANT_ALPHA, VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_ALPHA },
    };

    map<BlendOption, VkBlendOp> vkBlendOptionMap =
    {
        { BlendOption::ADD, VK_BLEND_OP_ADD }, { BlendOption::SUBTRACT, VK_BLEND_OP_SUBTRACT }, { BlendOption::REVERSE_SUBTRACT, VK_BLEND_OP_REVERSE_SUBTRACT },
        { BlendOption::MIN, VK_BLEND_OP_MIN }, { BlendOption::MAX,      VK_BLEND_OP_MAX      },
    };

    map<FaceCullOption, VkCullModeFlagBits> vkFaceCullOptionMap =
    {
        { FaceCullOption::Back, VK_CULL_MODE_BACK_BIT }, { FaceCullOption::Front,        VK_CULL_MODE_FRONT_BIT      },
        { FaceCullOption::None, VK_CULL_MODE_NONE     }, { FaceCullOption::FrontAndBack, VK_CULL_MODE_FRONT_AND_BACK },
    };

    map<CompareOption, VkCompareOp> vkDepthTestOptionMap =
    {
        { CompareOption::NEVER,         VK_COMPARE_OP_NEVER         }, { CompareOption::LESS,             VK_COMPARE_OP_LESS             },
        { CompareOption::ALWAYS,        VK_COMPARE_OP_ALWAYS        }, { CompareOption::GREATER,          VK_COMPARE_OP_GREATER          },
        { CompareOption::EQUAL,         VK_COMPARE_OP_EQUAL         }, { CompareOption::NOT_EQUAL,        VK_COMPARE_OP_NOT_EQUAL        },
        { CompareOption::LESS_OR_EQUAL, VK_COMPARE_OP_LESS_OR_EQUAL }, { CompareOption::GREATER_OR_EQUAL, VK_COMPARE_OP_GREATER_OR_EQUAL },
    };

    map<FrameBufferType, RenderPassType> vkFrameBufferTypeToRenderPassTypeMap =
    {
        { FrameBufferType::Present, RenderPassType::Present }, { FrameBufferType::Normal,        RenderPassType::Normal       },
        { FrameBufferType::Color,   RenderPassType::Color   }, { FrameBufferType::ShadowCubeMap, RenderPassType::ShadowCubeMap},
    };

    // 自定义的Debug回调函数，VKAPI_ATTR和VKAPI_CALL确保了正确的函数签名，从而被Vulkan调用
    static VKAPI_ATTR VkBool32 VKAPI_CALL VkDebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
    {
        std::cerr << "My debug call back: " << pCallbackData->pMessage << std::endl;

        // 如果是Error，立刻中断
        if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) 
            std::abort();

        // 返回一个布尔值，表明触发validation layer消息的Vulkan调用是否应被中止
        // 如果返回true，则调用将以VK_ERROR_VALIDATION_FAILED_EXT错误中止
        // 这通常用于测试validation layers本身，所以我们总是返回VK_FALSE
        return VK_FALSE;
    }

    RenderAPIVulkan::RenderAPIVulkan()
    {
        CreateVkInstance();
        CreateSurface();
        CreatePhysicalDevice();
        CreateLogicalDevice();
        CreateMemoryAllocator();
        CreateCommandPool();
        CreateSwapChain();
        CreateAllRenderPass();
        CreatePresentFrameBuffer();

        inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
        presentImageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
        {
            CreateVkFence(inFlightFences[i]);
            CreateVkSemaphore(presentImageAvailableSemaphores[i]);
        }

        InitImmediateCommand();
    }

    void RenderAPIVulkan::BeginFrame()
    {
        if (vkWaitForFences(device, 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX) != VK_SUCCESS)
			throw std::runtime_error("Failed to wait for fence!");

        // 检查是否有需要卸载的资源，并进行卸载
        // 这个函数调用时机比较关键，因为如果有资源被CommandBuffer引用了，那么必须在引用资源的CommandBuffer have completed execution之后才可以卸载
        // 这里写在刚刚WaitForFence之后，可以保证CommandBuffer此时的状态是满足have completed execution要求的
        CheckDeleteData();

        VkResult result = vkAcquireNextImageKHR(device, swapChain, UINT64_MAX, presentImageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &curPresentImageIdx);
        // 交换链和Surface已经不兼容了，不能继续用了，一般是窗口大小变化导致的
        if (result == VK_ERROR_OUT_OF_DATE_KHR)
            RecreateSwapChain();
        else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
            throw std::runtime_error("failed to acquire swap chain image!");

        vkResetFences(device, 1, &inFlightFences[currentFrame]);
    }

    void RenderAPIVulkan::EndFrame()
    {
        VkSwapchainKHR swapChains[] = { swapChain };
        VkSemaphore waitSemaphores[] = { presentImageAvailableSemaphores[currentFrame] };
        VkPresentInfoKHR presentInfo = {};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.pWaitSemaphores = waitSemaphores;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pSwapchains = swapChains;
        presentInfo.swapchainCount = 1;
        presentInfo.pImageIndices = &curPresentImageIdx;
        presentInfo.pResults = VK_NULL_HANDLE;
        
        VkResult result = vkQueuePresentKHR(presentQueue, &presentInfo);
        // VK_ERROR_OUT_OF_DATE_KHR表示交换链和Surface已经不兼容了，不能继续用了，必须重新创建交换链
        // VK_SUBOPTIMAL_KHR表示交换链还是可以继续用，但是和Surface的某些属性匹配得不是很好，不重新创建也行
        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || windowResized) 
            RecreateSwapChain();
        else if (result != VK_SUCCESS)
            throw std::runtime_error("failed to present swap chain image!");

        currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
    }

    void RenderAPIVulkan::OnWindowSizeChange(uint32_t width, uint32_t height)
    {
        newWindowWidth = width;
        newWindowHeight = height;
        windowResized = true;
    }

    void RenderAPIVulkan::SetRenderState(RenderStateSetting* state)
    {
        // Vulkan不需要实现这个接口
    }

    void RenderAPIVulkan::WaitForRenderFinish()
    {
        vkDeviceWaitIdle(device);
    }

    void RenderAPIVulkan::SwitchFrameBuffer(uint32_t id)
    {
        if (id == UINT32_MAX)
            curFBOIdx = presentFBOIdx;
        else
            curFBOIdx = id;
    }

    void RenderAPIVulkan::SetViewPort(unsigned int width, unsigned int height, unsigned int xOffset, unsigned int yOffset)
    {
        viewPortInfo.width = width;
        viewPortInfo.height = height;
        viewPortInfo.xOffset = xOffset;

        // 传入的参数是按0点在左下角的标准来的，Vulkan的0点在左上角，如果有偏移(编辑器模式)的话，Y轴偏移量要重新计算一下
        if (xOffset == 0 && yOffset == 0)
            viewPortInfo.yOffset = yOffset;
        else
            viewPortInfo.yOffset = ProjectSetting::srcHeight - height - yOffset;
    }

    void RenderAPIVulkan::ClearFrameBuffer()
    {
        // 这里实现了一个立刻Clear Frame Buffer的Vulkan版本，但是实际没有调用，因为在Vulkan里这样Clear是不太好的
        // 性能更好的做法是在BeginRenderPass中，加载FrameBuffer的时候去Clear，所以这里就无需Clear了，这个接口只是为了兼容OpenGL

        /*
        ImmediatelyExecute([=](VkCommandBuffer cmd)
        {
            auto FBO = GetFBOByIndex(curFBOIdx);

            VkRenderPassBeginInfo renderPassInfo = {};
            renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            renderPassInfo.renderPass = GetRenderPass(FBO->renderPassType);
            renderPassInfo.framebuffer = FBO->frameBuffers[GetCurFrameBufferIndex()];
            renderPassInfo.renderArea.offset = { viewPortInfo.xOffset, viewPortInfo.yOffset };
            renderPassInfo.renderArea.extent = { viewPortInfo.width, viewPortInfo.height };

            vkCmdBeginRenderPass(cmd, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

            vector<VkClearAttachment> clearAttachments = {};
            if (FBO->clearInfo.clearFlags & ZX_CLEAR_FRAME_BUFFER_COLOR_BIT)
            {
                VkClearValue clearValue = {};
                clearValue.color = { FBO->clearInfo.color.r, FBO->clearInfo.color.g, FBO->clearInfo.color.b, FBO->clearInfo.color.a };
                VkClearAttachment clearColorAttachment = {};
                clearColorAttachment.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                clearColorAttachment.colorAttachment = 0;
                clearColorAttachment.clearValue = clearValue;
                clearAttachments.push_back(clearColorAttachment);
            }
            if (FBO->clearInfo.clearFlags & ZX_CLEAR_FRAME_BUFFER_DEPTH_BIT)
            {
                VkClearValue clearValue = {};
                clearValue.depthStencil = { FBO->clearInfo.depth, 0 };
                VkClearAttachment clearDepthAttachment = {};
                clearDepthAttachment.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
                clearDepthAttachment.clearValue = clearValue;
                clearAttachments.push_back(clearDepthAttachment);
            }
            if (FBO->clearInfo.clearFlags & ZX_CLEAR_FRAME_BUFFER_STENCIL_BIT)
            {
                VkClearValue clearValue = {};
                clearValue.depthStencil = { 0, FBO->clearInfo.stencil };
                VkClearAttachment clearStencilAttachment = {};
                clearStencilAttachment.aspectMask = VK_IMAGE_ASPECT_STENCIL_BIT;
                clearStencilAttachment.clearValue = clearValue;
                clearAttachments.push_back(clearStencilAttachment);
            }

            VkClearRect clearRect = {};
            clearRect.baseArrayLayer = 0;
            if (FBO->bufferType == FrameBufferType::ShadowCubeMap)
                clearRect.layerCount = 6;
            else
                clearRect.layerCount = 1;

            clearRect.rect.offset = VkOffset2D { viewPortInfo.xOffset, viewPortInfo.yOffset };
            clearRect.rect.extent = VkExtent2D { viewPortInfo.width, viewPortInfo.height };

            vkCmdClearAttachments(cmd, static_cast<uint32_t>(clearAttachments.size()), clearAttachments.data(), 1, &clearRect);

            vkCmdEndRenderPass(cmd);
        });
        //*/
    }

    unsigned int RenderAPIVulkan::LoadTexture(const char* path, int& width, int& height)
    {
        int nrComponents;
        unsigned char* pixels = stbi_load(path, &width, &height, &nrComponents, STBI_rgb_alpha);

        VkDeviceSize imageSize = VkDeviceSize(width * height * 4);
        VulkanBuffer stagingBuffer = CreateBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_AUTO_PREFER_HOST, true);

        // 把数据拷贝到stagingBuffer
        void* data;
        void* pixelsPtr = pixels; // 为memcpy转换一下指针类型
        vmaMapMemory(vmaAllocator, stagingBuffer.allocation, &data);
        memcpy(data, pixelsPtr, static_cast<size_t>(imageSize));
        vmaUnmapMemory(vmaAllocator, stagingBuffer.allocation);

        stbi_image_free(pixels);

        uint32_t mipLevels = GetMipMapLevels(width, height);

        VulkanImage image = CreateImage(width, height, mipLevels, 1, VK_SAMPLE_COUNT_1_BIT,
            defaultImageFormat, VK_IMAGE_TILING_OPTIMAL,
            // 这里我们要从一个stagingBuffer接收数据，所以要写一个VK_IMAGE_USAGE_TRANSFER_DST_BIT
            // 又因为我们要生成mipmap，需要从这个原image读数据，所以又再加一个VK_IMAGE_USAGE_TRANSFER_SRC_BIT
            // 然后再写一个VK_IMAGE_USAGE_SAMPLED_BIT表示会用于shader代码采样纹理
            VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
            VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE);

        TransitionImageLayout(image.image, 
            // 从初始的Layout转换到接收stagingBuffer数据的Layout
            VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 
            // 从硬盘加载的图像默认都是Color
            VK_IMAGE_ASPECT_COLOR_BIT,
            // 转换可以直接开始，没有限制
            VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, 0,
            // 图像Transfer的写入操作需要在这个转换之后进行
            VK_PIPELINE_STAGE_TRANSFER_BIT, VK_ACCESS_TRANSFER_WRITE_BIT);

        // 把数据从stagingBuffer复制到image
        ImmediatelyExecute([=](VkCommandBuffer cmd)
        {
            VkBufferImageCopy region{};
            // 从buffer读取数据的起始偏移量
            region.bufferOffset = 0;
            // 这两个参数明确像素在内存里的布局方式，如果我们只是简单的紧密排列数据，就填0
            region.bufferRowLength = 0;
            region.bufferImageHeight = 0;
            // 下面4个参数都是在设置我们要把数据拷贝到image的哪一部分
            region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            region.imageSubresource.mipLevel = 0;
            region.imageSubresource.baseArrayLayer = 0;
            region.imageSubresource.layerCount = 1;
            // 这个也是在设置我们要把图像拷贝到哪一部分
            // 如果是整张图片，offset就全是0，extent就直接是图像高宽
            region.imageOffset = { 0, 0, 0 };
            region.imageExtent = { (uint32_t)width, (uint32_t)height, 1 };

            vkCmdCopyBufferToImage(
                cmd,
                stagingBuffer.buffer,
                image.image,
                // image当前的layout
                VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                1, &region
            );
        });

        GenerateMipMaps(image.image, defaultImageFormat, width, height, mipLevels);

        DestroyBuffer(stagingBuffer);

        VkImageView imageView = CreateImageView(image.image, defaultImageFormat, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_VIEW_TYPE_2D);
        VkSampler sampler = CreateSampler(mipLevels);

        return CreateVulkanTexture(image, imageView, sampler);
    }

    unsigned int RenderAPIVulkan::LoadCubeMap(const vector<string>& faces)
    {
        array<stbi_uc*, 6> textureData = {};
        int texWidth = 0, texHeight = 0, texChannels = 0;
        for (size_t i = 0; i < faces.size(); i++)
            textureData[i] = stbi_load(faces[i].c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);

        VkDeviceSize singleImageSize = VkDeviceSize(texWidth * texHeight * 4);

        VulkanImage image = CreateImage(texWidth, texHeight, 1, 6, VK_SAMPLE_COUNT_1_BIT,
            defaultImageFormat, VK_IMAGE_TILING_OPTIMAL,
            VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
            VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE);

        // 同LoadTexture
        TransitionImageLayout(image.image,
            VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            VK_IMAGE_ASPECT_COLOR_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, 0,
            VK_PIPELINE_STAGE_TRANSFER_BIT, VK_ACCESS_TRANSFER_WRITE_BIT);

        vector<VulkanBuffer> stagingBuffers;
        for (uint32_t i = 0; i < 6; i++)
            stagingBuffers.push_back(CreateBuffer(singleImageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_AUTO_PREFER_HOST, true));
        
        // 把数据从stagingBuffer复制到image
        ImmediatelyExecute([=](VkCommandBuffer cmd)
        {
            for (uint32_t i = 0; i < 6; i++)
            {
                // 把数据拷贝到stagingBuffer
                void* data;
                void* pixelsPtr = textureData[i]; // 为memcpy转换一下指针类型
                vmaMapMemory(vmaAllocator, stagingBuffers[i].allocation, &data);
                memcpy(data, pixelsPtr, static_cast<size_t>(singleImageSize));
                vmaUnmapMemory(vmaAllocator, stagingBuffers[i].allocation);

                stbi_image_free(textureData[i]);

                VkBufferImageCopy bufferCopyRegion = {};
                // 从buffer读取数据的起始偏移量
                bufferCopyRegion.bufferOffset = 0;
                // 这两个参数明确像素在内存里的布局方式，如果我们只是简单的紧密排列数据，就填0
                bufferCopyRegion.bufferRowLength = 0;
                bufferCopyRegion.bufferImageHeight = 0;
                // 下面4个参数都是在设置我们要把数据拷贝到image的哪一部分
                bufferCopyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                bufferCopyRegion.imageSubresource.mipLevel = 0;
                bufferCopyRegion.imageSubresource.baseArrayLayer = i;
                bufferCopyRegion.imageSubresource.layerCount = 1;
                bufferCopyRegion.imageOffset = { 0, 0, 0 };
                bufferCopyRegion.imageExtent = { static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight), 1 };

                vkCmdCopyBufferToImage(cmd,
                    stagingBuffers[i].buffer,
                    image.image,
                    // image当前的layout
                    VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                    1, &bufferCopyRegion
                );
            }
        });

        for (auto& stagingBuffer : stagingBuffers)
            DestroyBuffer(stagingBuffer);

        TransitionImageLayout(image.image,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
            VK_IMAGE_ASPECT_COLOR_BIT, 
            VK_PIPELINE_STAGE_TRANSFER_BIT, VK_ACCESS_TRANSFER_WRITE_BIT,
            VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, VK_ACCESS_SHADER_READ_BIT);

        VkImageView imageView = CreateImageView(image.image, defaultImageFormat, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_VIEW_TYPE_CUBE);
        VkSampler sampler = CreateSampler(1);

        return CreateVulkanTexture(image, imageView, sampler);
    }

    unsigned int RenderAPIVulkan::GenerateTextTexture(unsigned int width, unsigned int height, unsigned char* data)
    {
        // 一个文本像素8bit
        VkDeviceSize imageSize = VkDeviceSize(width * height);
        if (imageSize == 0)
            throw std::runtime_error("Can't create texture with size 0 !");
        VulkanBuffer stagingBuffer = CreateBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_AUTO_PREFER_HOST, true);

        // 把数据拷贝到stagingBuffer
        void* ptr;
        vmaMapMemory(vmaAllocator, stagingBuffer.allocation, &ptr);
        memcpy(ptr, data, static_cast<size_t>(imageSize));
        vmaUnmapMemory(vmaAllocator, stagingBuffer.allocation);

        VulkanImage image = CreateImage(width, height, 1, 1, VK_SAMPLE_COUNT_1_BIT,
            VK_FORMAT_R8_SRGB, VK_IMAGE_TILING_OPTIMAL,
            VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
            VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE);

        // 同LoadTexture
        TransitionImageLayout(image.image,
            VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            VK_IMAGE_ASPECT_COLOR_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, 0,
            VK_PIPELINE_STAGE_TRANSFER_BIT, VK_ACCESS_TRANSFER_WRITE_BIT);

        ImmediatelyExecute([=](VkCommandBuffer cmd)
        {
            VkBufferImageCopy region{};
            region.bufferOffset = 0;
            region.bufferRowLength = 0;
            region.bufferImageHeight = 0;
            region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            region.imageSubresource.mipLevel = 0;
            region.imageSubresource.baseArrayLayer = 0;
            region.imageSubresource.layerCount = 1;
            region.imageOffset = { 0, 0, 0 };
            region.imageExtent = { (uint32_t)width, (uint32_t)height, 1 };

            vkCmdCopyBufferToImage(cmd, stagingBuffer.buffer, image.image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
        });

        DestroyBuffer(stagingBuffer);

        TransitionImageLayout(image.image,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
            VK_IMAGE_ASPECT_COLOR_BIT,
            VK_PIPELINE_STAGE_TRANSFER_BIT, VK_ACCESS_TRANSFER_WRITE_BIT,
            VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, VK_ACCESS_SHADER_READ_BIT);

        VkImageView imageView = CreateImageView(image.image, VK_FORMAT_R8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_VIEW_TYPE_2D);
        VkSampler sampler = CreateSampler(1);

        return CreateVulkanTexture(image, imageView, sampler);
    }

    void RenderAPIVulkan::DeleteTexture(unsigned int id)
    {
        texturesToDelete.insert(pair(id, MAX_FRAMES_IN_FLIGHT));
    }

    ShaderReference* RenderAPIVulkan::LoadAndSetUpShader(const char* path, FrameBufferType type)
    {
        string shaderCode = Resources::LoadTextFile(path);
        auto shaderInfo = ShaderParser::GetShaderInfo(shaderCode, GraphicsAPI::Vulkan);

        uint32_t pipelineID = GetNextPipelineIndex();
        auto pipeline = GetPipelineByIndex(pipelineID);

        pipeline->name = path;
        pipeline->pipeline = CreatePipeline(path, shaderInfo, pipeline->descriptorSetLayout, pipeline->pipelineLayout, vkFrameBufferTypeToRenderPassTypeMap[type]);

        pipeline->inUse = true;

        ShaderReference* reference = new ShaderReference();
        reference->ID = pipelineID;
        reference->shaderInfo = shaderInfo;
        return reference;
    }

    uint32_t RenderAPIVulkan::CreateMaterialData()
    {

        uint32_t materialDataID = GetNextMaterialDataIndex();
        auto materialData = GetMaterialDataByIndex(materialDataID);

        materialData->inUse = true;

        return materialDataID;
    }

    void RenderAPIVulkan::UseMaterialData(uint32_t ID)
    {
        curMaterialDataIdx = ID;
    }

    // 这个函数完成的任务是通过shaderReference里的信息来配置MaterialData，也就是用vkUpdateDescriptorSets把Image与VkPipeline绑定起来
    void RenderAPIVulkan::SetUpMaterial(Material* material)
    {
        auto shaderReference = material->shader->reference;
        auto pipeline = GetPipelineByIndex(shaderReference->ID);

        auto vulkanMaterialData = GetMaterialDataByIndex(material->data->GetID());
        vulkanMaterialData->descriptorPool = CreateDescriptorPool(shaderReference->shaderInfo);

        // 创建Uniform Buffer
        for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
        {
            if (!shaderReference->shaderInfo.vertProperties.baseProperties.empty())
            {
                UniformBuffer uniformBuffer = CreateUniformBuffer(shaderReference->shaderInfo.vertProperties.baseProperties);
                vulkanMaterialData->vertUniformBuffers.push_back(uniformBuffer);
            }
            if (!shaderReference->shaderInfo.geomProperties.baseProperties.empty())
            {
                UniformBuffer uniformBuffer = CreateUniformBuffer(shaderReference->shaderInfo.geomProperties.baseProperties);
                vulkanMaterialData->geomUniformBuffers.push_back(uniformBuffer);
            }
            if (!shaderReference->shaderInfo.fragProperties.baseProperties.empty())
            {
                UniformBuffer uniformBuffer = CreateUniformBuffer(shaderReference->shaderInfo.fragProperties.baseProperties);
                vulkanMaterialData->fragUniformBuffers.push_back(uniformBuffer);
            }
        }

        // 把Uniform Buffer绑定到DescriptorSet上
        vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, pipeline->descriptorSetLayout);
        vulkanMaterialData->descriptorSets = CreateDescriptorSets(vulkanMaterialData->descriptorPool, layouts);
        for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
        {
            vector<VkWriteDescriptorSet> writeDescriptorSets;

            if (!vulkanMaterialData->vertUniformBuffers.empty())
            {
                VkDescriptorBufferInfo bufferInfo = {};
                bufferInfo.buffer = vulkanMaterialData->vertUniformBuffers[i].buffer.buffer;
                bufferInfo.offset = 0;
                bufferInfo.range = vulkanMaterialData->vertUniformBuffers[i].size;
                VkWriteDescriptorSet writeDescriptorSet = {};
                writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                writeDescriptorSet.dstSet = vulkanMaterialData->descriptorSets[i];
                writeDescriptorSet.dstBinding = vulkanMaterialData->vertUniformBuffers[i].binding;
                writeDescriptorSet.dstArrayElement = 0;
                writeDescriptorSet.descriptorCount = 1;
                writeDescriptorSet.pBufferInfo = &bufferInfo;
                writeDescriptorSets.push_back(writeDescriptorSet);
            }
            if (!vulkanMaterialData->geomUniformBuffers.empty())
            {
                VkDescriptorBufferInfo bufferInfo = {};
                bufferInfo.buffer = vulkanMaterialData->geomUniformBuffers[i].buffer.buffer;
                bufferInfo.offset = 0;
                bufferInfo.range = vulkanMaterialData->geomUniformBuffers[i].size;
                VkWriteDescriptorSet writeDescriptorSet = {};
                writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                writeDescriptorSet.dstSet = vulkanMaterialData->descriptorSets[i];
                writeDescriptorSet.dstBinding = vulkanMaterialData->geomUniformBuffers[i].binding;
                writeDescriptorSet.dstArrayElement = 0;
                writeDescriptorSet.descriptorCount = 1;
                writeDescriptorSet.pBufferInfo = &bufferInfo;
                writeDescriptorSets.push_back(writeDescriptorSet);
            }
            if (!vulkanMaterialData->fragUniformBuffers.empty())
            {
                VkDescriptorBufferInfo bufferInfo = {};
                bufferInfo.buffer = vulkanMaterialData->fragUniformBuffers[i].buffer.buffer;
                bufferInfo.offset = 0;
                bufferInfo.range = vulkanMaterialData->fragUniformBuffers[i].size;
                VkWriteDescriptorSet writeDescriptorSet = {};
                writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                writeDescriptorSet.dstSet = vulkanMaterialData->descriptorSets[i];
                writeDescriptorSet.dstBinding = vulkanMaterialData->fragUniformBuffers[i].binding;
                writeDescriptorSet.dstArrayElement = 0;
                writeDescriptorSet.descriptorCount = 1;
                writeDescriptorSet.pBufferInfo = &bufferInfo;
                writeDescriptorSets.push_back(writeDescriptorSet);
            }

            vkUpdateDescriptorSets(device, static_cast<uint32_t>(writeDescriptorSets.size()), writeDescriptorSets.data(), 0, nullptr);
        }

        // 把纹理绑定到DescriptorSet上
        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
        {
            vector<VkWriteDescriptorSet> writeDescriptorSets;

            for (auto& matTexture : material->data->textures)
            {
                uint32_t binding = UINT32_MAX;
                for (auto& textureProperty : shaderReference->shaderInfo.fragProperties.textureProperties)
                    if (matTexture.first == textureProperty.name)
                        binding = textureProperty.binding;

                if (binding == UINT32_MAX)
                    for (auto& textureProperty : shaderReference->shaderInfo.vertProperties.textureProperties)
                        if (matTexture.first == textureProperty.name)
                            binding = textureProperty.binding;

                if (binding == UINT32_MAX)
                {
                    Debug::LogError("No texture named " + matTexture.first + " matched !");
                    continue;
                }

                auto texture = GetTextureByIndex(matTexture.second->GetID());

                VkDescriptorImageInfo imageInfo{};
                imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                imageInfo.imageView = texture->imageView;
                imageInfo.sampler = texture->sampler;
                VkWriteDescriptorSet writeDescriptorSet = {};
                writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                writeDescriptorSet.dstSet = vulkanMaterialData->descriptorSets[i];
                writeDescriptorSet.dstBinding = binding;
                writeDescriptorSet.dstArrayElement = 0;
                writeDescriptorSet.descriptorCount = 1;
                writeDescriptorSet.pImageInfo = &imageInfo;

                writeDescriptorSets.push_back(writeDescriptorSet);
            }

            vkUpdateDescriptorSets(device, static_cast<uint32_t>(writeDescriptorSets.size()), writeDescriptorSets.data(), 0, nullptr);
        }

        // 设置材质数据
        for (auto& property : material->data->vec2Datas)
            SetShaderVector(material, property.first, property.second, true);
        for (auto& property : material->data->vec3Datas)
            SetShaderVector(material, property.first, property.second, true);
        for (auto& property : material->data->vec4Datas)
            SetShaderVector(material, property.first, property.second, true);
        for (auto& property : material->data->floatDatas)
            SetShaderScalar(material, property.first, property.second, true);
        for (auto& property : material->data->uintDatas)
            SetShaderScalar(material, property.first, property.second, true);

        material->data->initialized = true;
    }

    void RenderAPIVulkan::DeleteShader(uint32_t id)
    {
        pipelinesToDelete.insert(pair(id, MAX_FRAMES_IN_FLIGHT));
    }

    // Vulkan里不要立刻删除材质数据，因为Vulkan会同时处理多帧，调用删除的时候可能还有一帧正在并行处理
    // 所以这里只是把材质数据标记为删除，等到同时渲染的帧全部结束的时候再真正删除
    void RenderAPIVulkan::DeleteMaterialData(uint32_t id)
    {
        // 这里第二个参数的意思是这个材质要等这么多帧才能删除
        materialDatasToDelete.insert(pair(id, MAX_FRAMES_IN_FLIGHT ));
    }

    FrameBufferObject* RenderAPIVulkan::CreateFrameBufferObject(FrameBufferType type, unsigned int width, unsigned int height)
    {
        ClearInfo clearInfo = {};
        return CreateFrameBufferObject(type, clearInfo, width, height);
    }

    FrameBufferObject* RenderAPIVulkan::CreateFrameBufferObject(FrameBufferType type, const ClearInfo& clearInfo, unsigned int width, unsigned int height)
    {
        FrameBufferObject* FBO = new FrameBufferObject(type);
        FBO->clearInfo = clearInfo;
        FBO->isFollowWindow = width == 0 || height == 0;
        
        width = width == 0 ? GlobalData::srcWidth : width;
        height = height == 0 ? GlobalData::srcHeight : height;

        if (type == FrameBufferType::Normal)
        {
            FBO->ID = GetNextFBOIndex();
            FBO->ColorBuffer = GetNextAttachmentBufferIndex();
            auto colorAttachmentBuffer = GetAttachmentBufferByIndex(FBO->ColorBuffer);
            colorAttachmentBuffer->inUse = true;
            FBO->DepthBuffer = GetNextAttachmentBufferIndex();
            auto depthAttachmentBuffer = GetAttachmentBufferByIndex(FBO->DepthBuffer);
            depthAttachmentBuffer->inUse = true;

            auto vulkanFBO = GetFBOByIndex(FBO->ID);
            vulkanFBO->colorAttachmentIdx = FBO->ColorBuffer;
            vulkanFBO->depthAttachmentIdx = FBO->DepthBuffer;
            vulkanFBO->bufferType = FrameBufferType::Normal;
            vulkanFBO->renderPassType = RenderPassType::Normal;
            vulkanFBO->clearInfo = clearInfo;

            for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
            {
                // 这里很多Attachment后面都可能会用于其它Shader采样，所以都加了VK_IMAGE_USAGE_SAMPLED_BIT
                // 如果确定不会被用作采样的，可以不加VK_IMAGE_USAGE_SAMPLED_BIT，改成VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT提高性能
                VulkanImage colorImage = CreateImage(width, height, 1, 1, VK_SAMPLE_COUNT_1_BIT, defaultImageFormat, VK_IMAGE_TILING_OPTIMAL,
                    VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE);
                TransitionImageLayout(colorImage.image,
                    VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                    VK_IMAGE_ASPECT_COLOR_BIT, 
                    VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, 0,
                    VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT);
                VkImageView colorImageView = CreateImageView(colorImage.image, defaultImageFormat, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_VIEW_TYPE_2D);
                VkSampler colorSampler = CreateSampler(1);
                colorAttachmentBuffer->attachmentBuffers[i] = CreateVulkanTexture(colorImage, colorImageView, colorSampler);

                VulkanImage depthImage = CreateImage(width, height, 1, 1, VK_SAMPLE_COUNT_1_BIT, VK_FORMAT_D16_UNORM, VK_IMAGE_TILING_OPTIMAL,
                    VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE);
                TransitionImageLayout(depthImage.image,
                    VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                    VK_IMAGE_ASPECT_DEPTH_BIT,
                    VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, 0,
                    VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT, VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT);
                VkImageView depthImageView = CreateImageView(depthImage.image, VK_FORMAT_D16_UNORM, VK_IMAGE_ASPECT_DEPTH_BIT, VK_IMAGE_VIEW_TYPE_2D);
                VkSampler depthSampler = CreateSampler(1);
                depthAttachmentBuffer->attachmentBuffers[i] = CreateVulkanTexture(depthImage, depthImageView, depthSampler);

                array<VkImageView, 2> attachments = { colorImageView, depthImageView };

                VkFramebufferCreateInfo framebufferInfo{};
                framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
                // 指定可以兼容的render pass(这个frame buffer和指定的render pass的attachment的数量和类型需要一致)
                framebufferInfo.renderPass = GetRenderPass(RenderPassType::Normal);
                framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
                framebufferInfo.pAttachments = attachments.data();
                framebufferInfo.width = width;
                framebufferInfo.height = height;
                framebufferInfo.layers = 1;

                if (vkCreateFramebuffer(device, &framebufferInfo, nullptr, &vulkanFBO->frameBuffers[i]) != VK_SUCCESS)
                    throw std::runtime_error("failed to create framebuffer!");
            }

            vulkanFBO->inUse = true;
        }
        else if (type == FrameBufferType::Color)
        {
            FBO->ID = GetNextFBOIndex();
            FBO->ColorBuffer = GetNextAttachmentBufferIndex();
            auto colorAttachmentBuffer = GetAttachmentBufferByIndex(FBO->ColorBuffer);
            colorAttachmentBuffer->inUse = true;
            FBO->DepthBuffer = NULL;

            auto vulkanFBO = GetFBOByIndex(FBO->ID);
            vulkanFBO->colorAttachmentIdx = FBO->ColorBuffer;
            vulkanFBO->bufferType = FrameBufferType::Color;
            vulkanFBO->renderPassType = RenderPassType::Color;
            vulkanFBO->clearInfo = clearInfo;

            for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
            {
                VulkanImage colorImage = CreateImage(width, height, 1, 1, VK_SAMPLE_COUNT_1_BIT, defaultImageFormat, VK_IMAGE_TILING_OPTIMAL,
                    VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE);
                TransitionImageLayout(colorImage.image,
                    VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                    VK_IMAGE_ASPECT_COLOR_BIT,
                    VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, 0,
                    VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT);
                VkImageView colorImageView = CreateImageView(colorImage.image, defaultImageFormat, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_VIEW_TYPE_2D);
                VkSampler colorSampler = CreateSampler(1);
                colorAttachmentBuffer->attachmentBuffers[i] = CreateVulkanTexture(colorImage, colorImageView, colorSampler);

                array<VkImageView, 1> attachments = { colorImageView };

                VkFramebufferCreateInfo framebufferInfo{};
                framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
                framebufferInfo.renderPass = GetRenderPass(RenderPassType::Color);
                framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
                framebufferInfo.pAttachments = attachments.data();
                framebufferInfo.width = width;
                framebufferInfo.height = height;
                framebufferInfo.layers = 1;

                if (vkCreateFramebuffer(device, &framebufferInfo, nullptr, &vulkanFBO->frameBuffers[i]) != VK_SUCCESS)
                    throw std::runtime_error("failed to create framebuffer!");
            }

            vulkanFBO->inUse = true;
        }
        else if (type == FrameBufferType::ShadowCubeMap)
        {
            FBO->ID = GetNextFBOIndex();
            FBO->ColorBuffer = NULL;
            FBO->DepthBuffer = GetNextAttachmentBufferIndex();
            auto depthAttachmentBuffer = GetAttachmentBufferByIndex(FBO->DepthBuffer);
            depthAttachmentBuffer->inUse = true;

            auto vulkanFBO = GetFBOByIndex(FBO->ID);
            vulkanFBO->depthAttachmentIdx = FBO->DepthBuffer;
            vulkanFBO->bufferType = FrameBufferType::ShadowCubeMap;
            vulkanFBO->renderPassType = RenderPassType::ShadowCubeMap;
            vulkanFBO->clearInfo = clearInfo;

            for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
            {
                VulkanImage depthImage = CreateImage(width, height, 1, 6, VK_SAMPLE_COUNT_1_BIT, VK_FORMAT_D16_UNORM, VK_IMAGE_TILING_OPTIMAL,
                    VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE);
                TransitionImageLayout(depthImage.image,
                    VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                    VK_IMAGE_ASPECT_DEPTH_BIT,
                    VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, 0,
                    VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT, VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT);
                VkImageView depthImageView = CreateImageView(depthImage.image, VK_FORMAT_D16_UNORM, VK_IMAGE_ASPECT_DEPTH_BIT, VK_IMAGE_VIEW_TYPE_CUBE);
                VkSampler depthSampler = CreateSampler(1);
                depthAttachmentBuffer->attachmentBuffers[i] = CreateVulkanTexture(depthImage, depthImageView, depthSampler);

                array<VkImageView, 1> attachments = { depthImageView };

                VkFramebufferCreateInfo framebufferInfo{};
                framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
                framebufferInfo.renderPass = GetRenderPass(RenderPassType::ShadowCubeMap);
                framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
                framebufferInfo.pAttachments = attachments.data();
                framebufferInfo.width = width;
                framebufferInfo.height = height;
                framebufferInfo.layers = 6;

                if (vkCreateFramebuffer(device, &framebufferInfo, nullptr, &vulkanFBO->frameBuffers[i]) != VK_SUCCESS)
                    throw std::runtime_error("failed to create framebuffer!");
            }

            vulkanFBO->inUse = true;
        }
        else if (type == FrameBufferType::RayTracing)
        {
            FBO->ID = GetNextFBOIndex();
            FBO->ColorBuffer = GetNextAttachmentBufferIndex();
            auto colorAttachmentBuffer = GetAttachmentBufferByIndex(FBO->ColorBuffer);
            colorAttachmentBuffer->inUse = true;
            FBO->DepthBuffer = NULL;

            auto vulkanFBO = GetFBOByIndex(FBO->ID);
            vulkanFBO->colorAttachmentIdx = FBO->ColorBuffer;
            vulkanFBO->bufferType = FrameBufferType::Color;
            vulkanFBO->renderPassType = RenderPassType::Color;
            vulkanFBO->clearInfo = clearInfo;

            for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
            {
                VulkanImage colorImage = CreateImage(width, height, 1, 1, VK_SAMPLE_COUNT_1_BIT, defaultImageFormat, VK_IMAGE_TILING_OPTIMAL,
                    VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_STORAGE_BIT, VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE);
                TransitionImageLayout(colorImage.image,
                    VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                    VK_IMAGE_ASPECT_COLOR_BIT,
                    VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, 0,
                    VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT);
                VkImageView colorImageView = CreateImageView(colorImage.image, defaultImageFormat, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_VIEW_TYPE_2D);
                VkSampler colorSampler = CreateSampler(1);
                colorAttachmentBuffer->attachmentBuffers[i] = CreateVulkanTexture(colorImage, colorImageView, colorSampler);

                array<VkImageView, 1> attachments = { colorImageView };

                VkFramebufferCreateInfo framebufferInfo{};
                framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
                framebufferInfo.renderPass = GetRenderPass(RenderPassType::Color);
                framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
                framebufferInfo.pAttachments = attachments.data();
                framebufferInfo.width = width;
                framebufferInfo.height = height;
                framebufferInfo.layers = 1;

                if (vkCreateFramebuffer(device, &framebufferInfo, nullptr, &vulkanFBO->frameBuffers[i]) != VK_SUCCESS)
                    throw std::runtime_error("failed to create framebuffer!");
            }

            vulkanFBO->inUse = true;
        }
        else
        {
            Debug::LogError("Invalide frame buffer type.");
        }

        return FBO;
    }

    void RenderAPIVulkan::DeleteFrameBufferObject(FrameBufferObject* FBO)
    {
        DestroyFBOByIndex(FBO->ID);
	}

    uint32_t RenderAPIVulkan::AllocateDrawCommand(CommandType commandType)
    {
        uint32_t idx = GetNextDrawCommandIndex();
        auto drawCmd = GetDrawCommandByIndex(idx);
        drawCmd->commandType = commandType;

        if (commandType == CommandType::ShadowGeneration || commandType == CommandType::ForwardRendering ||
            commandType == CommandType::AfterEffectRendering || commandType == CommandType::UIRendering || 
            commandType == CommandType::AssetPreviewer || commandType == CommandType::RayTracing)
        {
            for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
            {
                VkSemaphore semaphore;
                CreateVkSemaphore(semaphore);
                drawCmd->drawCommands[i].signalSemaphores.push_back(semaphore);
            }
        }

        drawCmd->inUse = true;
        return idx;
    }

    void RenderAPIVulkan::Draw(uint32_t VAO)
    {
        drawIndexes.push_back({ .VAO = VAO, .pipelineID = curPipeLineIdx, .materialDataID = curMaterialDataIdx });
    }

    void RenderAPIVulkan::GenerateDrawCommand(uint32_t id)
    {
        auto curDrawCommandObj = GetDrawCommandByIndex(id);
        auto& curDrawCommand = curDrawCommandObj->drawCommands[currentFrame];
        auto commandBuffer = curDrawCommand.commandBuffer;

        vkResetCommandBuffer(commandBuffer, 0);

        VkCommandBufferBeginInfo beginInfo = {};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = 0;
        beginInfo.pInheritanceInfo = VK_NULL_HANDLE;
        if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
            throw std::runtime_error("failed to begin recording command buffer!");

        auto curFBO = GetFBOByIndex(curFBOIdx);
        VkRenderPassBeginInfo renderPassInfo = {};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = GetRenderPass(curFBO->renderPassType);
        renderPassInfo.framebuffer = curFBO->frameBuffers[GetCurFrameBufferIndex()];
        // 这个render area定义了shader将要加载和存储的位置
        renderPassInfo.renderArea.offset = { viewPortInfo.xOffset, viewPortInfo.yOffset };
        // 一般来说大小(extend)是和framebuffer的attachment一致的，如果小了会浪费，大了超出去的部分是一些未定义数值
        renderPassInfo.renderArea.extent = { viewPortInfo.width, viewPortInfo.height };
        if (curFBO->renderPassType == RenderPassType::Normal)
        {
            auto& clearInfo = curFBO->clearInfo;
            array<VkClearValue, 2> clearValues = {};
            clearValues[0].color = { { clearInfo.color.r, clearInfo.color.g, clearInfo.color.b, clearInfo.color.a } };
            clearValues[1].depthStencil = { clearInfo.depth, clearInfo.stencil };
            renderPassInfo.pClearValues = clearValues.data();
            renderPassInfo.clearValueCount = 2;
        }
        else if (curFBO->renderPassType == RenderPassType::ShadowCubeMap)
        {
            auto& clearInfo = curFBO->clearInfo;
            VkClearValue clearValue = {};
            clearValue.depthStencil.depth = clearInfo.depth;
            renderPassInfo.pClearValues = &clearValue;
            renderPassInfo.clearValueCount = 1;
        }
        else
        {
            renderPassInfo.pClearValues = VK_NULL_HANDLE;
            renderPassInfo.clearValueCount = 0;
        }
        vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        VkViewport viewport = {};
        viewport.x = static_cast<float>(viewPortInfo.xOffset);
        viewport.y = static_cast<float>(viewPortInfo.yOffset);
        viewport.width = static_cast<float>(viewPortInfo.width);
        viewport.height = static_cast<float>(viewPortInfo.height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

        VkRect2D scissor = {};
        scissor.offset = { viewPortInfo.xOffset, viewPortInfo.yOffset };
        scissor.extent = { viewPortInfo.width, viewPortInfo.height };
        vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

        for (auto& iter : drawIndexes)
        {
            auto vulkanVAO = GetVAOByIndex(iter.VAO);
            auto pipeline = GetPipelineByIndex(iter.pipelineID);
            auto materialData = GetMaterialDataByIndex(iter.materialDataID);

            VkBuffer vertexBuffers[] = { vulkanVAO->vertexBuffer };
            VkDeviceSize offsets[] = { 0 };
            vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

            vkCmdBindIndexBuffer(commandBuffer, vulkanVAO->indexBuffer, 0, VK_INDEX_TYPE_UINT32);

            vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->pipeline);

            vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->pipelineLayout, 0, 1, &materialData->descriptorSets[currentFrame], 0, VK_NULL_HANDLE);

            vkCmdDrawIndexed(commandBuffer, vulkanVAO->indexCount, 1, 0, 0, 0);
        }

        vkCmdEndRenderPass(commandBuffer);

        if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
            throw std::runtime_error("failed to record command buffer!");

        vector<VkPipelineStageFlags> waitStages = {};
        waitStages.resize(curWaitSemaphores.size(), VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT);
        VkSubmitInfo submitInfo = {};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.pCommandBuffers = &commandBuffer;
        submitInfo.commandBufferCount = 1;
        submitInfo.pWaitSemaphores = curWaitSemaphores.data();
        submitInfo.pWaitDstStageMask = waitStages.data();
        submitInfo.waitSemaphoreCount = static_cast<uint32_t>(curWaitSemaphores.size());
        submitInfo.pSignalSemaphores = curDrawCommand.signalSemaphores.data();
        submitInfo.signalSemaphoreCount = static_cast<uint32_t>(curDrawCommand.signalSemaphores.size());

        VkFence fence = VK_NULL_HANDLE;
#ifndef ZX_EDITOR
        // 在编辑器模式下最后一个Command固定是绘制编辑器UI的，在EditorGUIManager里提交Fence
        if (curDrawCommandObj->commandType == CommandType::UIRendering)
            fence = inFlightFences[currentFrame];
#endif
    
        if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, fence) != VK_SUCCESS)
            throw std::runtime_error("failed to submit draw command buffer!");

        // 当前这个命令激发的信号量就是下个命令需要等待的
        curWaitSemaphores = curDrawCommand.signalSemaphores;

        drawIndexes.clear();
    }

    void RenderAPIVulkan::DeleteMesh(unsigned int VAO)
    {
        meshsToDelete.insert(pair(VAO, MAX_FRAMES_IN_FLIGHT));
    }

    void RenderAPIVulkan::SetUpStaticMesh(unsigned int& VAO, const vector<Vertex>& vertices, const vector<uint32_t>& indices)
    {
        VAO = GetNextVAOIndex();
        auto meshBuffer = GetVAOByIndex(VAO);
        meshBuffer->indexCount = static_cast<uint32_t>(indices.size());
        meshBuffer->vertexCount = static_cast<uint32_t>(vertices.size());

        // ----------------------------------------------- Vertex Buffer -----------------------------------------------
        VkDeviceSize vertexBufferSize = sizeof(Vertex) * vertices.size();

        // 建立StagingBuffer
        VkBufferCreateInfo vertexStagingBufferInfo = {};
        vertexStagingBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        vertexStagingBufferInfo.size = vertexBufferSize;
        vertexStagingBufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        vertexStagingBufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        VmaAllocationCreateInfo vertexStagingAllocInfo = {};
        vertexStagingAllocInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_HOST;
        vertexStagingAllocInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;

        VkBuffer vertexStagingBuffer;
        VmaAllocation vertexStagingBufferAlloc;
        vmaCreateBuffer(vmaAllocator, &vertexStagingBufferInfo, &vertexStagingAllocInfo, &vertexStagingBuffer, &vertexStagingBufferAlloc, nullptr);

        // 拷贝数据到StagingBuffer
        void* vertexData;
        vmaMapMemory(vmaAllocator, vertexStagingBufferAlloc, &vertexData);
        memcpy(vertexData, vertices.data(), vertices.size() * sizeof(Vertex));
        vmaUnmapMemory(vmaAllocator, vertexStagingBufferAlloc);

        // 建立VertexBuffer
        VkBufferUsageFlags vertexBufferUsage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
        if (ProjectSetting::renderPipelineType == RenderPipelineType::RayTracing)
            vertexBufferUsage |= VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
        
        VkBufferCreateInfo vertexBufferInfo = {};
        vertexBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        vertexBufferInfo.size = vertexBufferSize;
        vertexBufferInfo.usage = vertexBufferUsage;
        vertexBufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE; // 只有一个队列簇使用

        VmaAllocationCreateInfo vertexBufferAllocInfo = {};
        vertexBufferAllocInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;

        vmaCreateBuffer(vmaAllocator, &vertexBufferInfo, &vertexBufferAllocInfo, &meshBuffer->vertexBuffer, &meshBuffer->vertexBufferAlloc, nullptr);

        // 如果是光追渲染管线，需要获取VertexBuffer的GPU地址
        if (ProjectSetting::renderPipelineType == RenderPipelineType::RayTracing)
        {
            VkBufferDeviceAddressInfo addressInfo = {};
            addressInfo.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
            addressInfo.buffer = meshBuffer->vertexBuffer;
            meshBuffer->vertexBufferDeviceAddress = vkGetBufferDeviceAddress(device, &addressInfo);
        }

        // 从StagingBuffer拷贝到VertexBuffer
        ImmediatelyExecute([=](VkCommandBuffer cmd)
        {
            VkBufferCopy copy = {};
            copy.dstOffset = 0;
            copy.srcOffset = 0;
            copy.size = vertexBufferSize;
            vkCmdCopyBuffer(cmd, vertexStagingBuffer, meshBuffer->vertexBuffer, 1, &copy);
        });

        // 销毁StagingBuffer
        vmaDestroyBuffer(vmaAllocator, vertexStagingBuffer, vertexStagingBufferAlloc);

        // ----------------------------------------------- Index Buffer -----------------------------------------------
        VkDeviceSize indexBufferSize = sizeof(unsigned int) * indices.size();

        // 建立StagingBuffer
        VkBufferCreateInfo indexStagingBufferInfo = {};
        indexStagingBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        indexStagingBufferInfo.size = indexBufferSize;
        indexStagingBufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        indexStagingBufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        VmaAllocationCreateInfo indexStagingAllocInfo = {};
        indexStagingAllocInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_HOST;
        indexStagingAllocInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;

        VkBuffer indexStagingBuffer;
        VmaAllocation indexStagingBufferAlloc;
        vmaCreateBuffer(vmaAllocator, &indexStagingBufferInfo, &indexStagingAllocInfo, &indexStagingBuffer, &indexStagingBufferAlloc, nullptr);

        // 拷贝数据到StagingBuffer
        void* indexData;
        vmaMapMemory(vmaAllocator, indexStagingBufferAlloc, &indexData);
        memcpy(indexData, indices.data(), indices.size() * sizeof(unsigned int));
        vmaUnmapMemory(vmaAllocator, indexStagingBufferAlloc);

        // 建立IndexBuffer
        VkBufferUsageFlags indexBufferUsage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
        if (ProjectSetting::renderPipelineType == RenderPipelineType::RayTracing)
            indexBufferUsage |= VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;

        VkBufferCreateInfo indexBufferInfo = {};
        indexBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        indexBufferInfo.size = indexBufferSize;
        indexBufferInfo.usage = indexBufferUsage;
        indexBufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE; // 只有一个队列簇使用

        VmaAllocationCreateInfo indexBufferAllocInfo = {};
        indexBufferAllocInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;

        vmaCreateBuffer(vmaAllocator, &indexBufferInfo, &indexBufferAllocInfo, &meshBuffer->indexBuffer, &meshBuffer->indexBufferAlloc, nullptr);

        // 如果是光追渲染管线，需要获取IndexBuffer的GPU地址
        if (ProjectSetting::renderPipelineType == RenderPipelineType::RayTracing)
        {
            VkBufferDeviceAddressInfo addressInfo = {};
            addressInfo.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
            addressInfo.buffer = meshBuffer->indexBuffer;
            meshBuffer->indexBufferDeviceAddress = vkGetBufferDeviceAddress(device, &addressInfo);
        }

        // 从StagingBuffer拷贝到IndexBuffer
        ImmediatelyExecute([=](VkCommandBuffer cmd)
        {
            VkBufferCopy copy = {};
            copy.dstOffset = 0;
            copy.srcOffset = 0;
            copy.size = indexBufferSize;
            vkCmdCopyBuffer(cmd, indexStagingBuffer, meshBuffer->indexBuffer, 1, &copy);
        });

        // 销毁StagingBuffer
        vmaDestroyBuffer(vmaAllocator, indexStagingBuffer, indexStagingBufferAlloc);

        // 如果是光追管线，还要创建一个BLAS( Bottom Level Acceleration Structure )
        if (ProjectSetting::renderPipelineType == RenderPipelineType::RayTracing)
        {
            BuildBottomLevelAccelerationStructure(VAO, true);
        }

        meshBuffer->inUse = true;
    }

    void RenderAPIVulkan::SetUpDynamicMesh(unsigned int& VAO, unsigned int vertexSize, unsigned int indexSize)
    {
        VAO = GetNextVAOIndex();
        auto meshBuffer = GetVAOByIndex(VAO);
        meshBuffer->indexCount = indexSize;
        meshBuffer->vertexCount = vertexSize;

        VmaAllocationCreateInfo vmaAllocInfo = {};
        vmaAllocInfo.usage = VMA_MEMORY_USAGE_AUTO;
        vmaAllocInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;

        // VertexBuffer
        VkDeviceSize vertexBufferSize = sizeof(Vertex) * vertexSize;

        VkBufferCreateInfo vertexBufferInfo = {};
        vertexBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        vertexBufferInfo.size = vertexBufferSize;
        vertexBufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
        vertexBufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        vmaCreateBuffer(vmaAllocator, &vertexBufferInfo, &vmaAllocInfo, &meshBuffer->vertexBuffer, &meshBuffer->vertexBufferAlloc, nullptr);
        vmaMapMemory(vmaAllocator, meshBuffer->vertexBufferAlloc, &meshBuffer->vertexBufferAddress);

        // IndexBuffer
        VkDeviceSize indexBufferSize = sizeof(unsigned int) * indexSize;

        VkBufferCreateInfo indexBufferInfo = {};
        indexBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        indexBufferInfo.size = indexBufferSize;
        indexBufferInfo.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
        indexBufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        vmaCreateBuffer(vmaAllocator, &indexBufferInfo, &vmaAllocInfo, &meshBuffer->indexBuffer, &meshBuffer->indexBufferAlloc, nullptr);
        vmaMapMemory(vmaAllocator, meshBuffer->indexBufferAlloc, &meshBuffer->indexBufferAddress);

        meshBuffer->inUse = true;
    }

    void RenderAPIVulkan::UpdateDynamicMesh(unsigned int VAO, const vector<Vertex>& vertices, const vector<uint32_t>& indices)
    {
        auto meshBuffer = GetVAOByIndex(VAO);

        memcpy(meshBuffer->vertexBufferAddress, vertices.data(), vertices.size() * sizeof(Vertex));
        memcpy(meshBuffer->indexBufferAddress, indices.data(), indices.size() * sizeof(unsigned int));
    }

    void RenderAPIVulkan::GenerateParticleMesh(unsigned int& VAO)
    {
        vector<Vertex> vertices =
        {
            { .Position = {  0.5f,  0.5f, 0.0f }, .TexCoords = { 1.0f, 0.0f } },
            { .Position = {  0.5f, -0.5f, 0.0f }, .TexCoords = { 1.0f, 1.0f } },
            { .Position = { -0.5f,  0.5f, 0.0f }, .TexCoords = { 0.0f, 0.0f } },
            { .Position = { -0.5f, -0.5f, 0.0f }, .TexCoords = { 0.0f, 1.0f } },
        };

        vector<uint32_t> indices =
        {
            2, 1, 3,
            2, 0, 1,
        };

        SetUpStaticMesh(VAO, vertices, indices);
    }

    uint32_t RenderAPIVulkan::CreateRayTracingPipeline(const RayTracingShaderPathGroup& rtShaderPathGroup)
    {
        VulkanRTPipeline* rtPipeline = new VulkanRTPipeline();
        rtPipeline->pipeline.name = "RayTracingPipeline";

        // 创建光追管线的DescriptorSetLayout
        vector<VkDescriptorSetLayoutBinding> bindings = {};

        // layout(set = 0, binding = 0)
        VkDescriptorSetLayoutBinding asBinding = {};
        asBinding.binding = 0;
        asBinding.descriptorType = VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR;
        asBinding.descriptorCount = 1;
        asBinding.stageFlags = VK_SHADER_STAGE_RAYGEN_BIT_KHR | VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR;
        bindings.push_back(asBinding);

        // layout(set = 0, binding = 1)
        VkDescriptorSetLayoutBinding imageBinding = {};
        imageBinding.binding = 1;
        imageBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
        imageBinding.descriptorCount = 1;
        imageBinding.stageFlags = VK_SHADER_STAGE_RAYGEN_BIT_KHR;
        bindings.push_back(imageBinding);

        rtPipeline->pipeline.descriptorSetLayout = CreateDescriptorSetLayout(bindings);

        // 创建场景资源的DescriptorSetLayout
        bindings.clear();

        // layout(set = 1, binding = 0)
        VkDescriptorSetLayoutBinding dataReferencesBinding = {};
        dataReferencesBinding.binding = 0;
        dataReferencesBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        dataReferencesBinding.descriptorCount = 1;
        dataReferencesBinding.stageFlags = VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR;
        bindings.push_back(dataReferencesBinding);

        // layout(set = 1, binding = 1)
        VkDescriptorSetLayoutBinding texturesBinding = {};
        texturesBinding.binding = 1;
        texturesBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        texturesBinding.descriptorCount = rtSceneTextureNum; // Todo: 这个数量应该是需要动态扩展的
        texturesBinding.stageFlags = VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR;
        bindings.push_back(texturesBinding);

        // layout(set = 1, binding = 2)
        VkDescriptorSetLayoutBinding cubeMapBinding = {};
        cubeMapBinding.binding = 2;
        cubeMapBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        cubeMapBinding.descriptorCount = rtSceneCubeMapNum; // Todo: 这个数量应该是需要动态扩展的
        cubeMapBinding.stageFlags = VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR | VK_SHADER_STAGE_MISS_BIT_KHR;
        bindings.push_back(cubeMapBinding);

        rtPipeline->pipeline.sceneDescriptorSetLayout = CreateDescriptorSetLayout(bindings);

        // 设置PushConstant
        vector<VkPushConstantRange> pushConstantRanges = {};
        VkPushConstantRange pushConstantRange = {};
        pushConstantRange.stageFlags = VK_SHADER_STAGE_RAYGEN_BIT_KHR | VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR | VK_SHADER_STAGE_MISS_BIT_KHR;
        pushConstantRange.offset = 0;
        pushConstantRange.size = sizeof(RayTracingPipelineConstants);
        pushConstantRanges.push_back(pushConstantRange);

        // 创建PipelineLayout
        vector<VkDescriptorSetLayout> descriptorSetLayouts = { rtPipeline->pipeline.descriptorSetLayout, rtPipeline->pipeline.sceneDescriptorSetLayout };
        rtPipeline->pipeline.pipelineLayout = CreatePipelineLayout(descriptorSetLayouts, pushConstantRanges);

        // 创建Shader Module和Shader Groups
        vector<VkPipelineShaderStageCreateInfo> stages = {};
        vector<VkRayTracingShaderGroupCreateInfoKHR> shaderGroups = {};

        VkPipelineShaderStageCreateInfo stageInfo = {};
        stageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        stageInfo.pName = "main";

        VkRayTracingShaderGroupCreateInfoKHR shaderGroupInfo = {};
        shaderGroupInfo.sType = VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_KHR;

        uint32_t shaderIndex = 0;
        for (auto& path : rtShaderPathGroup.rGenPaths)
        {
			stageInfo.stage = VK_SHADER_STAGE_RAYGEN_BIT_KHR;
			stageInfo.module = CreateShaderModule(Resources::LoadBinaryFile(Resources::GetAssetFullPath(path + ".spv")));
			stages.push_back(stageInfo);

            shaderGroupInfo.type = VK_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL_KHR;
            shaderGroupInfo.generalShader      = shaderIndex++;
            shaderGroupInfo.closestHitShader   = VK_SHADER_UNUSED_KHR;
            shaderGroupInfo.anyHitShader       = VK_SHADER_UNUSED_KHR;
            shaderGroupInfo.intersectionShader = VK_SHADER_UNUSED_KHR;
            shaderGroups.push_back(shaderGroupInfo);
		}
        for (auto& path : rtShaderPathGroup.rMissPaths)
        {
            stageInfo.stage = VK_SHADER_STAGE_MISS_BIT_KHR;
            stageInfo.module = CreateShaderModule(Resources::LoadBinaryFile(Resources::GetAssetFullPath(path + ".spv")));
            stages.push_back(stageInfo);

            shaderGroupInfo.type = VK_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL_KHR;
            shaderGroupInfo.generalShader      = shaderIndex++;
            shaderGroupInfo.closestHitShader   = VK_SHADER_UNUSED_KHR;
            shaderGroupInfo.anyHitShader       = VK_SHADER_UNUSED_KHR;
            shaderGroupInfo.intersectionShader = VK_SHADER_UNUSED_KHR;
            shaderGroups.push_back(shaderGroupInfo);
        }
        for (auto& groupPath : rtShaderPathGroup.rHitGroupPaths)
        {
            stageInfo.stage = VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR;
			stageInfo.module = CreateShaderModule(Resources::LoadBinaryFile(Resources::GetAssetFullPath(groupPath.rClosestHitPath + ".spv")));
			stages.push_back(stageInfo);

            shaderGroupInfo.type = groupPath.rIntersectionPath.empty() ? VK_RAY_TRACING_SHADER_GROUP_TYPE_TRIANGLES_HIT_GROUP_KHR : VK_RAY_TRACING_SHADER_GROUP_TYPE_PROCEDURAL_HIT_GROUP_KHR;
            shaderGroupInfo.generalShader      = VK_SHADER_UNUSED_KHR;
			shaderGroupInfo.closestHitShader   = shaderIndex++;
			shaderGroupInfo.anyHitShader       = VK_SHADER_UNUSED_KHR;
			shaderGroupInfo.intersectionShader = VK_SHADER_UNUSED_KHR;

            if (!groupPath.rAnyHitPath.empty())
            {
				stageInfo.stage = VK_SHADER_STAGE_ANY_HIT_BIT_KHR;
				stageInfo.module = CreateShaderModule(Resources::LoadBinaryFile(Resources::GetAssetFullPath(groupPath.rAnyHitPath + ".spv")));
				stages.push_back(stageInfo);

				shaderGroupInfo.anyHitShader = shaderIndex++;
            }

            if (!groupPath.rIntersectionPath.empty())
            {
                stageInfo.stage = VK_SHADER_STAGE_INTERSECTION_BIT_KHR;
                stageInfo.module = CreateShaderModule(Resources::LoadBinaryFile(Resources::GetAssetFullPath(groupPath.rIntersectionPath + ".spv")));
                stages.push_back(stageInfo);

                shaderGroupInfo.intersectionShader = shaderIndex++;
            }

			shaderGroups.push_back(shaderGroupInfo);
        }

        // 创建Pipeline
        VkRayTracingPipelineCreateInfoKHR rayPipelineInfo{};
        rayPipelineInfo.sType = VK_STRUCTURE_TYPE_RAY_TRACING_PIPELINE_CREATE_INFO_KHR;
        rayPipelineInfo.stageCount = static_cast<uint32_t>(stages.size());
        rayPipelineInfo.pStages = stages.data();
        rayPipelineInfo.groupCount = static_cast<uint32_t>(shaderGroups.size());
        rayPipelineInfo.pGroups = shaderGroups.data();
        rayPipelineInfo.maxPipelineRayRecursionDepth = 2;
        rayPipelineInfo.layout = rtPipeline->pipeline.pipelineLayout;
        if (vkCreateRayTracingPipelinesKHR(device, VK_NULL_HANDLE, VK_NULL_HANDLE, 1, &rayPipelineInfo, nullptr, &rtPipeline->pipeline.pipeline) != VK_SUCCESS)
			throw std::runtime_error("Failed to create ray tracing pipeline!");

        // 创建完成后，立刻清理Shader Module
        for (auto& stage : stages)
			vkDestroyShaderModule(device, stage.module, nullptr);

        // 开始创建Shader Binding Table
        // 一个SBT可以看作是4个数组，分别是Ray Gen, Miss, Hit, Callable这4种类型的Shader Group数组

        uint32_t rGenCount  = static_cast<uint32_t>(rtShaderPathGroup.rGenPaths.size());
        uint32_t rMissCount = static_cast<uint32_t>(rtShaderPathGroup.rMissPaths.size());
        uint32_t rHitCount  = static_cast<uint32_t>(rtShaderPathGroup.rHitGroupPaths.size());
        uint32_t shaderHandleCount = rGenCount + rMissCount + rHitCount;

        // 这是一个Shader引用在SBT上的大小
        uint32_t shaderHandleSize = rtPhysicalProperties.shaderGroupHandleSize;
        // 这是Shader引用在SBT上的对齐大小，注意这个对齐的大小和实际大小可能不一样，所以这里要把查询到的实际大小去对齐查询到的对齐大小
        // 有可能最后Shader引用实际大小比对齐大小更小，这会导致SBT Buffer上有内存碎片，可以把这些碎片空间利用起来存放shaderRecordEXT数据
        // 除了Shader Group内的Shader引用要按这个对齐，不同的Shader Group之间也要按这个对齐
        uint32_t shaderHandleAlignment = Math::AlignUp(shaderHandleSize, rtPhysicalProperties.shaderGroupHandleAlignment);
        // Shader Group之间要按这个大小对齐
        uint32_t shaderGroupAlignment = rtPhysicalProperties.shaderGroupBaseAlignment;

        // Ray Gen比较特殊，size和stride要一致
        rtPipeline->SBT.raygenRegion.size = static_cast<VkDeviceSize>(Math::AlignUp(shaderHandleAlignment, shaderGroupAlignment));
        rtPipeline->SBT.raygenRegion.stride = rtPipeline->SBT.raygenRegion.size;
        // 整个Shader Group要对齐VkPhysicalDeviceRayTracingPipelinePropertiesKHR::shaderGroupBaseAlignment
        rtPipeline->SBT.missRegion.size = static_cast<VkDeviceSize>(Math::AlignUp(shaderHandleAlignment * rMissCount, shaderGroupAlignment));
        rtPipeline->SBT.missRegion.stride = shaderHandleAlignment;
        rtPipeline->SBT.hitRegion.size = static_cast<VkDeviceSize>(Math::AlignUp(shaderHandleAlignment * rHitCount, shaderGroupAlignment));
        rtPipeline->SBT.hitRegion.stride = shaderHandleAlignment;

        // 获取绑定到光线追踪VkPipeline上的Shader Handles
        uint32_t dataSize = shaderHandleCount * shaderHandleSize;
        vector<uint8_t> handles(dataSize);
        if (vkGetRayTracingShaderGroupHandlesKHR(device, rtPipeline->pipeline.pipeline, 0, shaderHandleCount, dataSize, handles.data()) != VK_SUCCESS)
            throw std::runtime_error("Failed to get shader group handles!");

        // 创建SBT Buffer
        VkDeviceSize sbtBufferSize = rtPipeline->SBT.raygenRegion.size + rtPipeline->SBT.missRegion.size + rtPipeline->SBT.hitRegion.size;
        rtPipeline->SBT.buffer = CreateBuffer(sbtBufferSize,
            VK_BUFFER_USAGE_SHADER_BINDING_TABLE_BIT_KHR | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VMA_MEMORY_USAGE_AUTO_PREFER_HOST, true);

        // 获取每个Shader Group的起始地址
        VkBufferDeviceAddressInfo sbtBufferInfo = {};
        sbtBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
        sbtBufferInfo.buffer = rtPipeline->SBT.buffer.buffer;
        VkDeviceAddress sbtBufferAddress = vkGetBufferDeviceAddress(device, &sbtBufferInfo);
        rtPipeline->SBT.raygenRegion.deviceAddress = sbtBufferAddress;
        rtPipeline->SBT.missRegion.deviceAddress = sbtBufferAddress + rtPipeline->SBT.raygenRegion.size;
        rtPipeline->SBT.hitRegion.deviceAddress = sbtBufferAddress + rtPipeline->SBT.raygenRegion.size + rtPipeline->SBT.missRegion.size;

        // 获取管线中的Shader地址
        uint8_t* shaderHandlePtr = handles.data();
        // 获取SBT Buffer的映射地址
        uint8_t* sbtBufferPtr = static_cast<uint8_t*>(rtPipeline->SBT.buffer.mappedAddress);
        // SBT Buffer的临时指针
        uint8_t* tmpPtr = sbtBufferPtr;
        // 把Ray Gen Shader拷贝到SBT Buffer上
        memcpy(tmpPtr, shaderHandlePtr, shaderHandleSize);
        // 移到下一个Shader地址
        shaderHandlePtr += shaderHandleSize;

        // 移到下一个Shader Group地址(Miss)
        tmpPtr = sbtBufferPtr + rtPipeline->SBT.raygenRegion.size;
        // 遍历拷贝Miss Shader到SBT Buffer上
        for (uint32_t i = 0; i < rMissCount; ++i)
        {
            memcpy(tmpPtr, shaderHandlePtr, shaderHandleSize);
            // 移到下一个Shader地址
            shaderHandlePtr += shaderHandleSize;
            // 移到当前Shader Group中的下一个地址
            tmpPtr += rtPipeline->SBT.missRegion.stride;
        }

        // 移到下一个Shader Group地址(Hit)
        tmpPtr = sbtBufferPtr + rtPipeline->SBT.raygenRegion.size + rtPipeline->SBT.missRegion.size;
        // 遍历拷贝Hit Shader到SBT Buffer上
        for (uint32_t i = 0; i < rHitCount; ++i)
        {
            memcpy(tmpPtr, shaderHandlePtr, shaderHandleSize);
            // 移到下一个Shader地址
            shaderHandlePtr += shaderHandleSize;
            // 移到当前Shader Group中的下一个地址
            tmpPtr += rtPipeline->SBT.hitRegion.stride;
        }

        rtPipelines.push_back(rtPipeline);
        uint32_t rtPipelineID = static_cast<uint32_t>(rtPipelines.size() - 1);

        // 创建管线要使用的固定DescriptorSet
        CreateRTPipelineData(rtPipelineID);
        // 初始化光追场景资源
        CreateRTSceneData(rtPipelineID);
        // 初始化TLAS数组
        rtPipeline->tlasIdx = GetNextTLASGroupIndex();
        auto tlasGroup = GetTLASGroupByIndex(rtPipeline->tlasIdx);
        tlasGroup->asGroup.resize(MAX_FRAMES_IN_FLIGHT);
        tlasGroup->inUse = true;
        // 初始化构建TLAS的中间Buffer
        rtTLASStagingBuffers.resize(MAX_FRAMES_IN_FLIGHT);
        rtTLASScratchBuffers.resize(MAX_FRAMES_IN_FLIGHT);
        rtTLASInstanceBuffers.resize(MAX_FRAMES_IN_FLIGHT);

        return rtPipelineID;
    }

    void RenderAPIVulkan::SwitchRayTracingPipeline(uint32_t rtPipelineID)
    {
        curRTPipelineID = rtPipelineID;

        rtVPMatrix.clear();
        rtVPMatrix.resize(MAX_FRAMES_IN_FLIGHT);
        rtFrameCount.clear();
        rtFrameCount.resize(MAX_FRAMES_IN_FLIGHT, 0);
    }

    uint32_t RenderAPIVulkan::CreateRayTracingMaterialData()
    {
        uint32_t rtMaterialDataID = GetNextRTMaterialDataIndex();
        auto rtMaterialData = GetRTMaterialDataByIndex(rtMaterialDataID);

        rtMaterialData->inUse = true;

        return rtMaterialDataID;
    }

    void RenderAPIVulkan::SetUpRayTracingMaterialData(Material* material)
    {
        auto vulkanRTMaterialData = GetRTMaterialDataByIndex(material->data->GetRTID());

        ShaderParser::SetUpRTMaterialData(material->data, GraphicsAPI::Vulkan);

        vulkanRTMaterialData->buffers.resize(MAX_FRAMES_IN_FLIGHT);

        VkDeviceSize bufferSize = static_cast<VkDeviceSize>(material->data->rtMaterialDataSize);
        if (bufferSize > 0)
        {
            for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
            {
                // 这个Buffer可能是一次性创建不再修改的，可以考虑优化成GPU Only的
                vulkanRTMaterialData->buffers[i] = CreateBuffer(bufferSize, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VMA_MEMORY_USAGE_AUTO, true, true);
            }
        }

        for (auto& property : material->data->vec2Datas)
            SetShaderVector(material, property.first, property.second, true);
        for (auto& property : material->data->vec3Datas)
            SetShaderVector(material, property.first, property.second, true);
        for (auto& property : material->data->vec4Datas)
            SetShaderVector(material, property.first, property.second, true);
        for (auto& property : material->data->floatDatas)
            SetShaderScalar(material, property.first, property.second, true);
        for (auto& property : material->data->uintDatas)
            SetShaderScalar(material, property.first, property.second, true);
    }

    void RenderAPIVulkan::DeleteRayTracingMaterialData(uint32_t id)
    {
        rtMaterialDatasToDelete.insert(pair(id, MAX_FRAMES_IN_FLIGHT));
    }

    void RenderAPIVulkan::SetRayTracingSkyBox(uint32_t textureID)
    {
        // 天空盒纹理默认是当前这一帧的第一个CubeMap
        curRTSceneCubeMapIndexMap[textureID] = 0;
        curRTSceneCubeMapIndexes.push_back(textureID);
    }

    void RenderAPIVulkan::PushRayTracingMaterialData(Material* material)
    {
        // 把这个材质使用的纹理添加到当前光追场景中的总纹理列表中
        for (auto& iter : material->data->textures)
        {
            auto textureID = iter.second->GetID();
            if (iter.second->type == TextureType::ZX_2D)
            {
                if (curRTSceneTextureIndexMap.find(textureID) == curRTSceneTextureIndexMap.end())
                {
                    curRTSceneTextureIndexMap[textureID] = static_cast<uint32_t>(curRTSceneTextureIndexes.size());
                    curRTSceneTextureIndexes.emplace_back(textureID);
                }
            }
            else if (iter.second->type == TextureType::ZX_Cube)
            {
                if (curRTSceneCubeMapIndexMap.find(textureID) == curRTSceneCubeMapIndexMap.end())
                {
                    curRTSceneCubeMapIndexMap[textureID] = static_cast<uint32_t>(curRTSceneCubeMapIndexes.size());
                    curRTSceneCubeMapIndexes.emplace_back(textureID);
				}
			}
        }

        // 把这个光追材质添加到当前光追场景中的总光追材质列表中
        auto rtMaterialDataID = material->data->GetRTID();
        if (curRTSceneRTMaterialDataMap.find(rtMaterialDataID) == curRTSceneRTMaterialDataMap.end())
        {
            curRTSceneRTMaterialDataMap[rtMaterialDataID] = static_cast<uint32_t>(curRTSceneRTMaterialDatas.size());
            curRTSceneRTMaterialDatas.emplace_back(rtMaterialDataID);
		}

        // 更新当前帧的光追材质Buffer数据
        auto vulkanRTMaterialData = GetRTMaterialDataByIndex(rtMaterialDataID);
        auto& buffer = vulkanRTMaterialData->buffers[currentFrame];
        uint8_t* tmpPtr = static_cast<uint8_t*>(buffer.mappedAddress);

        // 遍历纹理，并把引用索引写入Buffer
        for (auto& iter : material->data->textures)
        {
            auto textureID = iter.second->GetID();
            auto textureIdx = 0;

            if (iter.second->type == TextureType::ZX_2D)
				textureIdx = curRTSceneTextureIndexMap[textureID];
			else if (iter.second->type == TextureType::ZX_Cube)
				textureIdx = curRTSceneCubeMapIndexMap[textureID];

            SetShaderScalar(material, iter.first, textureIdx);
        }
    }

    void RenderAPIVulkan::PushAccelerationStructure(uint32_t VAO, uint32_t hitGroupIdx, uint32_t rtMaterialDataID, const Matrix4& transform)
    {
        VulkanASInstanceData asIns = {};
        asIns.VAO = VAO;
        asIns.hitGroupIdx = hitGroupIdx;
        asIns.rtMaterialDataID = rtMaterialDataID;
        asIns.transform = transform;
        asInstanceData.push_back(std::move(asIns));
    }

    void RenderAPIVulkan::RayTrace(uint32_t commandID, const RayTracingPipelineConstants& rtConstants)
    {
        auto rtPipeline = rtPipelines[curRTPipelineID];

        // 计算画面静止的帧数，累积式光追渲染需要这个数据
        if (rtConstants.VP != rtVPMatrix[currentFrame])
        {
            rtFrameCount[currentFrame] = 0;
            rtVPMatrix[currentFrame] = rtConstants.VP;
        }
        uint32_t frameCount = rtFrameCount[currentFrame]++;

        // 先更新当前帧和光追管线绑定的场景数据
        UpdateRTSceneData(curRTPipelineID);
        // 更新当前帧和光追管线绑定的管线数据
        UpdateRTPipelineData(curRTPipelineID);

        // 获取当前帧的Command Buffer
		auto curDrawCommandObj = GetDrawCommandByIndex(commandID);
		auto& curDrawCommand = curDrawCommandObj->drawCommands[currentFrame];
		auto commandBuffer = curDrawCommand.commandBuffer;

		// 重置Command Buffer
		vkResetCommandBuffer(commandBuffer, 0);

		// 开始记录Command Buffer
		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = 0;
		beginInfo.pInheritanceInfo = VK_NULL_HANDLE;
		if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
			throw std::runtime_error("Failed to begin recording command buffer!");

        // 获取光追管线输出的目标图像
        auto curFBO = GetFBOByIndex(curFBOIdx);
        uint32_t textureID = GetAttachmentBufferByIndex(curFBO->colorAttachmentIdx)->attachmentBuffers[currentFrame];
        auto texture = GetTextureByIndex(textureID);

        // 转为光追输出格式
        TransitionImageLayout(commandBuffer, texture->image.image, 
            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_ASPECT_COLOR_BIT, 
            VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT, VK_ACCESS_SHADER_READ_BIT,
            VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_KHR, VK_ACCESS_SHADER_WRITE_BIT);

		// 绑定光追管线
		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR, rtPipeline->pipeline.pipeline);
		// 绑定光追管线描述符集
        vector<VkDescriptorSet> rtSets{ rtPipeline->pipelineData.descriptorSets[currentFrame], rtPipeline->sceneData.descriptorSets[currentFrame] };
        vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR, rtPipeline->pipeline.pipelineLayout,
            0, static_cast<uint32_t>(rtSets.size()), rtSets.data(), 0, nullptr);

        // 转一下Push Constants数据格式，按std140内存布局规则存放
        // 这里因为是固定数据，就简单手动处理了，没像Uniform Buffer那样写一个通用的函数
        void* pushConstants = malloc(208); // (16 * 3 + 3 + 1) * 4
        char* ptr = static_cast<char*>(pushConstants);
        if (ptr != NULL)
        {
            float matVP[16];
            rtConstants.VP.ToColumnMajorArray(matVP);
            memcpy(ptr, matVP, 16 * sizeof(float));
            ptr += 16 * sizeof(float);

            float matIV[16];
            rtConstants.V_Inv.ToColumnMajorArray(matIV);
            memcpy(ptr, matIV, 16 * sizeof(float));
            ptr += 16 * sizeof(float);

            float matIP[16];
            rtConstants.P_Inv.ToColumnMajorArray(matIP);
            memcpy(ptr, matIP, 16 * sizeof(float));
            ptr += 16 * sizeof(float);

            memcpy(ptr, &rtConstants.lightPos, 3 * sizeof(float));
            ptr += 3 * sizeof(float);

            memcpy(ptr, &frameCount, sizeof(uint32_t));
        }

        // 绑定光追管线常量
        vkCmdPushConstants(commandBuffer, rtPipeline->pipeline.pipelineLayout, 
            VK_SHADER_STAGE_RAYGEN_BIT_KHR | VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR | VK_SHADER_STAGE_MISS_BIT_KHR,
            0, sizeof(float) * 52, pushConstants);

        free(pushConstants);

		// Ray Trace
		vkCmdTraceRaysKHR(commandBuffer, 
            &rtPipeline->SBT.raygenRegion, &rtPipeline->SBT.missRegion, &rtPipeline->SBT.hitRegion, &rtPipeline->SBT.callableRegion,
            viewPortInfo.width, viewPortInfo.height, 1);

        // 转为Shader读取格式
        TransitionImageLayout(commandBuffer, texture->image.image, 
            VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_ASPECT_COLOR_BIT, 
            VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_KHR, VK_ACCESS_SHADER_WRITE_BIT,
            VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT, VK_ACCESS_SHADER_READ_BIT);

		// 结束记录Command Buffer
		if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
			throw std::runtime_error("Failed to record command buffer!");

        // 提交Command Buffer
        vector<VkPipelineStageFlags> waitStages = {};
        waitStages.resize(curWaitSemaphores.size(), VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_KHR);
        VkSubmitInfo submitInfo = {};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.pCommandBuffers = &commandBuffer;
        submitInfo.commandBufferCount = 1;
        submitInfo.pWaitSemaphores = curWaitSemaphores.data();
        submitInfo.pWaitDstStageMask = waitStages.data();
        submitInfo.waitSemaphoreCount = static_cast<uint32_t>(curWaitSemaphores.size());
        submitInfo.pSignalSemaphores = curDrawCommand.signalSemaphores.data();
        submitInfo.signalSemaphoreCount = static_cast<uint32_t>(curDrawCommand.signalSemaphores.size());
        if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS)
            throw std::runtime_error("Failed to submit draw command buffer!");

        curWaitSemaphores = curDrawCommand.signalSemaphores;

        // 清空当前帧的场景数据
        asInstanceData.clear();
        curRTSceneTextureIndexes.clear();
        curRTSceneTextureIndexMap.clear();
        curRTSceneCubeMapIndexes.clear();
        curRTSceneCubeMapIndexMap.clear();
        curRTSceneRTMaterialDatas.clear();
        curRTSceneRTMaterialDataMap.clear();
    }

    void RenderAPIVulkan::BuildTopLevelAccelerationStructure(uint32_t commandID)
    {
        // 获取当前帧的Command Buffer
        auto curDrawCommandObj = GetDrawCommandByIndex(commandID);
        auto& curDrawCommand = curDrawCommandObj->drawCommands[currentFrame];
        auto commandBuffer = curDrawCommand.commandBuffer;

        // 重置Command Buffer
        if (vkResetCommandBuffer(commandBuffer, 0) != VK_SUCCESS)
            throw std::runtime_error("Failed to reset command buffer!");

        // 开始记录Command Buffer
        VkCommandBufferBeginInfo beginInfo = {};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = 0;
        beginInfo.pInheritanceInfo = VK_NULL_HANDLE;
        if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
            throw std::runtime_error("Failed to begin recording command buffer!");

        auto& curTLAS = GetTLASGroupByIndex(rtPipelines[curRTPipelineID]->tlasIdx)->asGroup[currentFrame];
        const bool isUpdate = curTLAS.isBuilt;

        // 场景中要渲染的对象实例数据
        vector<VkAccelerationStructureInstanceKHR> instances;

        for (size_t i = 0; i < asInstanceData.size(); i++)
        {
            auto& data = asInstanceData[i];
            auto meshData = GetVAOByIndex(data.VAO);

            VkAccelerationStructureInstanceKHR asIns = {};
            asIns.transform = GetVkTransformMatrix(data.transform);
            asIns.instanceCustomIndex = i;
            asIns.mask = 0xFF;
            asIns.instanceShaderBindingTableRecordOffset = data.hitGroupIdx;
            asIns.flags = VK_GEOMETRY_INSTANCE_TRIANGLE_FACING_CULL_DISABLE_BIT_KHR;
            asIns.accelerationStructureReference = meshData->blas.deviceAddress;

            instances.push_back(asIns);
        }

        uint32_t insNum = static_cast<uint32_t>(instances.size());
        VkDeviceSize insBufferSize = sizeof(VkAccelerationStructureInstanceKHR) * insNum;

        // 建立StagingBuffer
        if (!isUpdate)
            rtTLASStagingBuffers[currentFrame] = CreateBuffer(insBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_AUTO_PREFER_HOST, true);

        // 拷贝场景实例数据到StagingBuffer
        memcpy(rtTLASStagingBuffers[currentFrame].mappedAddress, instances.data(), insBufferSize);

        // 存放场景实例数据的Buffer
        if (!isUpdate)
            rtTLASInstanceBuffers[currentFrame] = CreateBuffer(insBufferSize, 
                VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
            	VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE, false, true);

        // 把数据从stagingBuffer拷贝到instancesBuffer
        VkBufferCopy copy = {};
        copy.dstOffset = 0;
        copy.srcOffset = 0;
        copy.size = insBufferSize;
        vkCmdCopyBuffer(commandBuffer, rtTLASStagingBuffers[currentFrame].buffer, rtTLASInstanceBuffers[currentFrame].buffer, 1, &copy);

        // 确保构建TLAS之前，数据拷贝已完成
        VkMemoryBarrier barrier = {};
        barrier.sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER;
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_ACCELERATION_STRUCTURE_WRITE_BIT_KHR;
        vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_KHR,
            0, 1, &barrier, 0, nullptr, 0, nullptr);

        // 添加一些描述，包装一下上面的Instances Buffer数据，用于构建TLAS
        VkAccelerationStructureGeometryInstancesDataKHR instancesData = {};
        instancesData.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_INSTANCES_DATA_KHR;
        instancesData.data.deviceAddress = rtTLASInstanceBuffers[currentFrame].deviceAddress;
        VkAccelerationStructureGeometryKHR tlasGeometry = {};
        tlasGeometry.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR;
        tlasGeometry.geometryType = VK_GEOMETRY_TYPE_INSTANCES_KHR;
        tlasGeometry.geometry.instances = instancesData;

        // 构建TLAS的信息
        VkAccelerationStructureBuildGeometryInfoKHR buildInfo = {};
        buildInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
        buildInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR;
        buildInfo.flags = VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR | VK_BUILD_ACCELERATION_STRUCTURE_ALLOW_UPDATE_BIT_KHR;
        buildInfo.mode = isUpdate ? VK_BUILD_ACCELERATION_STRUCTURE_MODE_UPDATE_KHR : VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR;
        buildInfo.geometryCount = 1;
        buildInfo.pGeometries = &tlasGeometry;

        // 获取TLAS的构建所需的Buffer大小
        VkAccelerationStructureBuildSizesInfoKHR sizeInfo = {};
        sizeInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_SIZES_INFO_KHR;
        vkGetAccelerationStructureBuildSizesKHR(device, VK_ACCELERATION_STRUCTURE_BUILD_TYPE_DEVICE_KHR, &buildInfo, &insNum, &sizeInfo);

        // 新建TLAS
        if (!isUpdate)
        {
            // 创建TLAS Buffer
            curTLAS.buffer = CreateBuffer(sizeInfo.accelerationStructureSize,
                VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_STORAGE_BIT_KHR | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE, false, true);

            // 创建TLAS的信息(仅创建，不填充数据，所以只需要Buffer和Size)
            VkAccelerationStructureCreateInfoKHR createInfo = {};
            createInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_KHR;
            createInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR;
            createInfo.size = sizeInfo.accelerationStructureSize;
            createInfo.buffer = curTLAS.buffer.buffer;

            // 创建TLAS
            if (vkCreateAccelerationStructureKHR(device, &createInfo, nullptr, &curTLAS.as) != VK_SUCCESS)
                throw std::runtime_error("Create acceleration structure failed!");
        }

        // 创建Scratch Buffer，Vulkan构建TLAS需要一个Buffer来放中间数据
        if (!isUpdate)
            rtTLASScratchBuffers[currentFrame] = CreateBuffer(sizeInfo.buildScratchSize,
                VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
                VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE, false, true);

        // 继续填充构建TLAS需要的信息
        buildInfo.srcAccelerationStructure = isUpdate ? curTLAS.as : VK_NULL_HANDLE;
        buildInfo.dstAccelerationStructure = curTLAS.as;
        buildInfo.scratchData.deviceAddress = rtTLASScratchBuffers[currentFrame].deviceAddress;

        // 本次构建TLAS的所需的数据范围
        VkAccelerationStructureBuildRangeInfoKHR buildRangeInfo = {};
        buildRangeInfo.firstVertex = 0;
        buildRangeInfo.primitiveCount = insNum;
        buildRangeInfo.primitiveOffset = 0;
        buildRangeInfo.transformOffset = 0;
        const VkAccelerationStructureBuildRangeInfoKHR* pBuildRangeInfo = &buildRangeInfo;

        // 构建TLAS
        vkCmdBuildAccelerationStructuresKHR(commandBuffer, 1, &buildInfo, &pBuildRangeInfo);
        curTLAS.isBuilt = true;

        // 结束Command Buffer的记录
        if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
            throw std::runtime_error("failed to record command buffer!");

        // 提交Command Buffer
        vector<VkPipelineStageFlags> waitStages = {};
        waitStages.resize(curWaitSemaphores.size(), VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_KHR);
        VkSubmitInfo submitInfo = {};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.pCommandBuffers = &commandBuffer;
        submitInfo.commandBufferCount = 1;
        submitInfo.pWaitSemaphores = curWaitSemaphores.data();
        submitInfo.pWaitDstStageMask = waitStages.data();
        submitInfo.waitSemaphoreCount = static_cast<uint32_t>(curWaitSemaphores.size());
        submitInfo.pSignalSemaphores = curDrawCommand.signalSemaphores.data();
        submitInfo.signalSemaphoreCount = static_cast<uint32_t>(curDrawCommand.signalSemaphores.size());
        if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS)
            throw std::runtime_error("Failed to submit draw command buffer!");

        curWaitSemaphores = curDrawCommand.signalSemaphores;
    }

    void RenderAPIVulkan::BuildBottomLevelAccelerationStructure(uint32_t VAO, bool isCompact)
    {
        auto meshBuffer = GetVAOByIndex(VAO);

        // 三角形信息里填的vertexFormat固定为VK_FORMAT_R32G32B32_SFLOAT，如果某些特殊设备float不是4字节这里直接抛出异常
        // 没做适配处理，因为绝大部分的环境下的float都是4字节
        if (sizeof(float) != 4)
            throw std::runtime_error("float size is not 4");

        // 三角形Mesh数据，主要包含了顶点和索引Buffer地址
        VkAccelerationStructureGeometryTrianglesDataKHR triangles = {};
        triangles.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_TRIANGLES_DATA_KHR;
        triangles.vertexFormat = VK_FORMAT_R32G32B32_SFLOAT;
        triangles.vertexData.deviceAddress = meshBuffer->vertexBufferDeviceAddress;
        triangles.vertexStride = sizeof(Vertex);
        triangles.maxVertex = meshBuffer->vertexCount;
        triangles.indexType = VK_INDEX_TYPE_UINT32;
        triangles.indexData.deviceAddress = meshBuffer->indexBufferDeviceAddress;

        // 模型的几何体信息，主要是引用上面那个三角形Mesh数据
        VkAccelerationStructureGeometryKHR geometry = {};
        geometry.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR;
        geometry.geometryType = VK_GEOMETRY_TYPE_TRIANGLES_KHR;
        geometry.flags = VK_GEOMETRY_OPAQUE_BIT_KHR;
        geometry.geometry.triangles = triangles;

        // 这个RangeInfo在描述我们要构建的BLAS，是用的前面引用的顶点数据Buffer里的哪一段
        VkAccelerationStructureBuildRangeInfoKHR rangeInfo = {};
        rangeInfo.firstVertex = 0;
        rangeInfo.primitiveCount = meshBuffer->indexCount / 3;
        rangeInfo.primitiveOffset = 0;
        rangeInfo.transformOffset = 0;

        // 这是要构建一个BLAS所需要的信息
        VkAccelerationStructureBuildGeometryInfoKHR buildInfo = {};
        buildInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
        // 指定创建的是BLAS还是TLAS，这里创建BLAS
        buildInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;
        // 创建全新的BLAS，还是更新现有的BLAS
        buildInfo.mode = VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR;
        buildInfo.flags = VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR;
        if (isCompact)
            buildInfo.flags |= VK_BUILD_ACCELERATION_STRUCTURE_ALLOW_COMPACTION_BIT_KHR;
        // 这里传一个几何体数组，允许把多个几何体放到一个BLAS里，但是一般来说模型和BLAS是一对一的
        buildInfo.pGeometries = &geometry;
        buildInfo.geometryCount = 1;

        // 调用Vulkan的接口来查询当前要创建的BLAS大小
        // 调用一次这个接口只能查询一个BLAS的大小，所以参数只传递一个VkAccelerationStructureBuildGeometryInfoKHR
        // 第4个参数pMaxPrimitiveCounts传递的是一个uint32_t数组，这个数组是对应我们传入的VkAccelerationStructureBuildGeometryInfoKHR
        // 里面的pGeometries，明确每个几何体的图元数量
        // 最后一个参数是用来返回查询数据的
        vector<uint32_t> maxPrimCount = { rangeInfo.primitiveCount };
        VkAccelerationStructureBuildSizesInfoKHR sizeInfo = {};
        sizeInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_SIZES_INFO_KHR;
        vkGetAccelerationStructureBuildSizesKHR(device, VK_ACCELERATION_STRUCTURE_BUILD_TYPE_DEVICE_KHR, &buildInfo, maxPrimCount.data(), &sizeInfo);

        // 重新计算Scratch Buffer大小，先向上对齐查询到的硬件最小对齐量，再加一个这个大小
        // 这个计算是为了配合后面取Scratch Buffer地址时，需要处理的一个奇怪的问题，后面取地址时详细解释
        VkDeviceSize realScratchSize = Math::AlignUpPOT(sizeInfo.buildScratchSize, static_cast<VkDeviceSize>(physicalAccelerationStructureProperties.minAccelerationStructureScratchOffsetAlignment));
        realScratchSize += physicalAccelerationStructureProperties.minAccelerationStructureScratchOffsetAlignment;

        // 创建一个Scratch Buffer，这个是给Vulkan创建BLAS用的临时Buffer，因为Vulkan在创建BLAS的过程中会产生一些中间数据
        // 其实Vulkan完全可以自己创建这个Buffer用完了再自己销毁，但是出于性能考虑Vulkan选择让用户把Scratch Buffer创建好再提供给它
        // 因为假设要创建100个BLAS，而Vulkan在接口内部自己创建和销毁临时Buffer，就要做100次Buffer创建和销毁
        // 但是如果丢给用户创建，用户可以创建一个Scratch Buffer，然后用这一个Buffer调用100次创建BLAS的接口，这样只需要一次创建和销毁
        // 所以这里Vulkan其实更推荐集中多个模型一起创建BLAS，因为这样可以共用一个Scratch Buffer，而不是创建一个模型就马上创建一个BLAS
        // 如果要集中创建BLAS复用Scratch Buffer，那这个Scratch Buffer的大小按最大的模型Size来创建
        VulkanBuffer scratchBuffer = CreateBuffer(realScratchSize,
            VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
            VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE);
        VkBufferDeviceAddressInfo bufferInfo = {};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
        bufferInfo.buffer = scratchBuffer.buffer;
        bufferInfo.pNext = nullptr;
        VkDeviceAddress scratchAddress = vkGetBufferDeviceAddress(device, &bufferInfo);

        // 这里取到地址后，向上对齐VkPhysicalDeviceAccelerationStructurePropertiesKHR::minAccelerationStructureScratchOffsetAlignment
        // vkCmdBuildAccelerationStructuresKHR 要求传入的 Scratch Buffer 地址要对齐这个查询到的最小对齐量
        // 但是vkGetBufferDeviceAddress接口拿到的地址并不能保证这个对齐，如果地址没满足要求，就会报错直接Crash
        // 我遇到的情况是，如果启动引擎，加载的第一个场景就是光追场景，是不会有问题的，但是如果从其它场景切换到一个光追场景，就容易遇到这个问题
        // 网上没怎么搜到这个问题的处理方案，我自己想了一个处理方式，就是创建 Scratch Buffer 的时候建大一点
        // 要至少大一个最小对齐量，然后这里计算传给 vkCmdBuildAccelerationStructuresKHR 的地址，向上对齐这个最小对齐量
        // 这样如果拿到的地址没对齐，相当于手动向后偏移一点去对齐了
        // 但是这样的话Buffer前面一小段空间就无法使用了，如果直接按查询到的 buildScratchSize 来构建Buffer，然后又向后偏移了一点
        // 就有可能导致实际可用的Buffer空间不够，所以前面重新计算了实际构建 Scratch Buffer 的大小，至少大一个对齐量
        // 这样就能保证即使向后偏移了一小段地址，总的Buffer空间也一定是够用的
        scratchAddress = Math::AlignUpPOT(scratchAddress, static_cast<VkDeviceAddress>(physicalAccelerationStructureProperties.minAccelerationStructureScratchOffsetAlignment));

        // 创建一个存放BLAS数据的VkBuffer，Size为前面查询到的，所需的大小
        meshBuffer->blas.buffer = CreateBuffer(sizeInfo.accelerationStructureSize,
            VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_STORAGE_BIT_KHR | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
            VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE);

        // 创建一个BLAS要用到的信息，主要参数是存放BLAS的Buffer和Size
        VkAccelerationStructureCreateInfoKHR blasInfo = {};
        blasInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_KHR;
        blasInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;
        blasInfo.size = sizeInfo.accelerationStructureSize;
        blasInfo.buffer = meshBuffer->blas.buffer.buffer;

        // 创建BLAS，注意这里创建好后只是初始化状态，真正的数据还要后续填充
        if (vkCreateAccelerationStructureKHR(device, &blasInfo, nullptr, &meshBuffer->blas.as) != VK_SUCCESS)
            throw std::runtime_error("Create acceleration structure failed!");

        // 继续填充构建BLAS所需的信息
        // 把刚刚创建的，处于初始状态的BLAS传给dstAccelerationStructure，表示这是我们所要构建的BLAS
        buildInfo.dstAccelerationStructure = meshBuffer->blas.as;
        // 如果我们不是创建一个全新的BLAS，而是通过一个现有的BLAS来更新当前的这个BLAS，就需要设置这个参数
        buildInfo.srcAccelerationStructure = VK_NULL_HANDLE;
        // 把前面创建的Scratch Buffer提供给Vulkan
        buildInfo.scratchData.deviceAddress = scratchAddress;

        // 等后面正真构建完BLAS后，实际占用的内存大小可能会比之前计算的要小，所以这里可以选择压缩一下BLAS
        // 就是按实际占用大小再重新创建一个BLAS，节省内存空间，然后销毁之前创建的BLAS
        // 查询构建完成后的BLAS真实大小，需要用到VkQueryPool
        VkQueryPool queryPool = VK_NULL_HANDLE;
        if (isCompact)
        {
            VkQueryPoolCreateInfo queryInfo = {};
            queryInfo.sType = VK_STRUCTURE_TYPE_QUERY_POOL_CREATE_INFO;
            queryInfo.queryCount = 1;
            queryInfo.queryType = VK_QUERY_TYPE_ACCELERATION_STRUCTURE_COMPACTED_SIZE_KHR;
            vkCreateQueryPool(device, &queryInfo, nullptr, &queryPool);
        }

        ImmediatelyExecute([=](VkCommandBuffer cmd)
        {
            // 用前面准备的 VkAccelerationStructureBuildGeometryInfoKHR 正真构建BLAS
            // 参数2,3是传递一个 VkAccelerationStructureBuildGeometryInfoKHR 数组
            // 和其它Vulkan接口一样，这里可以一次调用创建多个，这里暂时只传一个
            // 这里的最后一个参数，是一个 VkAccelerationStructureBuildRangeInfoKHR 的二维数组
            // 数组的第一层对应参数2,3的 VkAccelerationStructureBuildGeometryInfoKHR 数组
            // 数组的第二层对应每个 VkAccelerationStructureBuildGeometryInfoKHR 里的 pGeometries 数组
            // 所以二维数组里的每一个RangeInfo都最终对应了一个 VkAccelerationStructureGeometryKHR
            vector<VkAccelerationStructureBuildRangeInfoKHR> ranges = { rangeInfo };
            const VkAccelerationStructureBuildRangeInfoKHR* ptr = ranges.data();
            vkCmdBuildAccelerationStructuresKHR(cmd, 1, &buildInfo, &ptr);

            // 如果一次性创建多个BLAS，然后共用Scratch Buffer，可能会有读写冲突问题，加个Barrier
            // 不过这里暂时一次只创建一个，其实没必要
            VkMemoryBarrier barrier = {};
            barrier.sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER;
            barrier.srcAccessMask = VK_ACCESS_ACCELERATION_STRUCTURE_WRITE_BIT_KHR;
            barrier.dstAccessMask = VK_ACCESS_ACCELERATION_STRUCTURE_READ_BIT_KHR;
            vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_KHR,
                VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_KHR, 0, 1, &barrier, 0, nullptr, 0, nullptr);

            if (isCompact)
            {
                vkResetQueryPool(device, queryPool, 0, 1);
                // 查一下我们刚刚构建好的BLAS实际占用大小，把结果放到queryPool里(这个接口也可以传数组一次性查多个数据)
                vkCmdWriteAccelerationStructuresPropertiesKHR(cmd, 1, &buildInfo.dstAccelerationStructure,
                    VK_QUERY_TYPE_ACCELERATION_STRUCTURE_COMPACTED_SIZE_KHR, queryPool, 0);
            }
        });

        if (isCompact)
        {
            // 从queryPool里获取我们刚刚查的BLAS实际大小数据
            vector<VkDeviceSize> compactSizes(1);
            vkGetQueryPoolResults(device, queryPool, 0, static_cast<uint32_t>(compactSizes.size()),
                compactSizes.size() * sizeof(VkDeviceSize), compactSizes.data(), sizeof(VkDeviceSize), VK_QUERY_RESULT_WAIT_BIT);
            sizeInfo.accelerationStructureSize = compactSizes[0];

            // 用实际占用大小重新创建一个BLAS Buffer
            VulkanBuffer newBLASBuffer = CreateBuffer(sizeInfo.accelerationStructureSize,
                VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_STORAGE_BIT_KHR | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
                VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE);

            // 新的压缩过的Buffer和Size信息
            VkAccelerationStructureCreateInfoKHR newBLASInfo = {};
            newBLASInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_KHR;
            newBLASInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;
            newBLASInfo.size = sizeInfo.accelerationStructureSize;
            newBLASInfo.buffer = newBLASBuffer.buffer;

            // 创建新的压缩过的BLAS
            VkAccelerationStructureKHR newAS = {};
            if (vkCreateAccelerationStructureKHR(device, &newBLASInfo, nullptr, &newAS) != VK_SUCCESS)
                throw std::runtime_error("Create acceleration structure failed!");

            ImmediatelyExecute([=](VkCommandBuffer cmd)
            {
                // 把之前构建好的BLAS数据，复制到新的，大小精确不浪费的BLAS上
                VkCopyAccelerationStructureInfoKHR copyInfo = {};
                copyInfo.sType = VK_STRUCTURE_TYPE_COPY_ACCELERATION_STRUCTURE_INFO_KHR;
                copyInfo.src = buildInfo.dstAccelerationStructure;
                copyInfo.dst = newAS;
                copyInfo.mode = VK_COPY_ACCELERATION_STRUCTURE_MODE_COMPACT_KHR;
                vkCmdCopyAccelerationStructureKHR(cmd, &copyInfo);
            });

            // 销毁之前的BLAS
            DestroyBuffer(meshBuffer->blas.buffer);
            vkDestroyAccelerationStructureKHR(device, meshBuffer->blas.as, nullptr);

            // 把新创建的BLAS赋值过来
            meshBuffer->blas.as = newAS;
            meshBuffer->blas.buffer = newBLASBuffer;
        }

        // 获取BLAS的Device Address
        VkAccelerationStructureDeviceAddressInfoKHR addressInfo = {};
        addressInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_DEVICE_ADDRESS_INFO_KHR;
        addressInfo.accelerationStructure = meshBuffer->blas.as;
        meshBuffer->blas.deviceAddress = vkGetAccelerationStructureDeviceAddressKHR(device, &addressInfo);
        meshBuffer->blas.isBuilt = true;

        // BLAS创建完成后立刻销毁Scratch Buffer
        DestroyBuffer(scratchBuffer);
    }

    void RenderAPIVulkan::UseShader(unsigned int ID)
    {
        curPipeLineIdx = ID;
    }

    // Boolean
    void RenderAPIVulkan::SetShaderScalar(Material* material, const string& name, bool value, bool allBuffer)
    {
        if (allBuffer)
        {
            vector<void*> valueAddresses;

            if (material->type == MaterialType::RayTracing)
                valueAddresses = GetRTMaterialPropertyAddressAllBuffer(material->data, name);
            else
                valueAddresses = GetShaderPropertyAddressAllBuffer(material->shader->reference, material->data->GetID(), name);

            for (auto valueAddress : valueAddresses)
                memcpy(valueAddress, &value, sizeof(value));
        }
        else
        {
            void* valueAddress = nullptr;

            if (material->type == MaterialType::RayTracing)
                valueAddress = GetRTMaterialPropertyAddress(material->data, name);
            else
                valueAddress = GetShaderPropertyAddress(material->shader->reference, material->data->GetID(), name);

            if (valueAddress != nullptr)
                memcpy(valueAddress, &value, sizeof(value));
        }
    }

    // Integer
    void RenderAPIVulkan::SetShaderScalar(Material* material, const string& name, int value, bool allBuffer)
    {
        if (allBuffer)
        {
            vector<void*> valueAddresses;

            if (material->type == MaterialType::RayTracing)
                valueAddresses = GetRTMaterialPropertyAddressAllBuffer(material->data, name);
            else
                valueAddresses = GetShaderPropertyAddressAllBuffer(material->shader->reference, material->data->GetID(), name);

            for (auto valueAddress : valueAddresses)
                memcpy(valueAddress, &value, sizeof(value));
        }
        else
        {
            void* valueAddress = nullptr;

            if (material->type == MaterialType::RayTracing)
                valueAddress = GetRTMaterialPropertyAddress(material->data, name);
            else
                valueAddress = GetShaderPropertyAddress(material->shader->reference, material->data->GetID(), name);

            if (valueAddress != nullptr)
                memcpy(valueAddress, &value, sizeof(value));
        }
    }

    // Float
    void RenderAPIVulkan::SetShaderScalar(Material* material, const string& name, float value, bool allBuffer)
    {
        if (allBuffer)
        {
            vector<void*> valueAddresses;

            if (material->type == MaterialType::RayTracing)
                valueAddresses = GetRTMaterialPropertyAddressAllBuffer(material->data, name);
            else
                valueAddresses = GetShaderPropertyAddressAllBuffer(material->shader->reference, material->data->GetID(), name);

            for (auto valueAddress : valueAddresses)
                memcpy(valueAddress, &value, sizeof(value));
        }
        else
        {
            void* valueAddress = nullptr;

            if (material->type == MaterialType::RayTracing)
                valueAddress = GetRTMaterialPropertyAddress(material->data, name);
            else
                valueAddress = GetShaderPropertyAddress(material->shader->reference, material->data->GetID(), name);

            if (valueAddress != nullptr)
                memcpy(valueAddress, &value, sizeof(value));
        }
    }

    // Unsigned Integer
    void RenderAPIVulkan::SetShaderScalar(Material* material, const string& name, uint32_t value, bool allBuffer)
    {
        if (allBuffer)
        {
            vector<void*> valueAddresses;

            if (material->type == MaterialType::RayTracing)
                valueAddresses = GetRTMaterialPropertyAddressAllBuffer(material->data, name);
            else
                valueAddresses = GetShaderPropertyAddressAllBuffer(material->shader->reference, material->data->GetID(), name);

            for (auto valueAddress : valueAddresses)
                memcpy(valueAddress, &value, sizeof(value));
        }
        else
        {
            void* valueAddress = nullptr;

            if (material->type == MaterialType::RayTracing)
                valueAddress = GetRTMaterialPropertyAddress(material->data, name);
            else
                valueAddress = GetShaderPropertyAddress(material->shader->reference, material->data->GetID(), name);

            if (valueAddress != nullptr)
                memcpy(valueAddress, &value, sizeof(value));
        }
    }

    // Vector2
    void RenderAPIVulkan::SetShaderVector(Material* material, const string& name, const Vector2& value, bool allBuffer)
    {
        SetShaderVector(material, name, value, 0, allBuffer);
    }
    void RenderAPIVulkan::SetShaderVector(Material* material, const string& name, const Vector2& value, uint32_t idx, bool allBuffer)
    {
        float* array = new float[2];
        value.ToArray(array);
        if (allBuffer)
        {
            vector<void*> valueAddresses;

            if (material->type == MaterialType::RayTracing)
                valueAddresses = GetRTMaterialPropertyAddressAllBuffer(material->data, name, idx);
            else
                valueAddresses = GetShaderPropertyAddressAllBuffer(material->shader->reference, material->data->GetID(), name, idx);

            for (auto valueAddress : valueAddresses)
                memcpy(valueAddress, array, sizeof(float) * 2);
        }
        else
        {
            void* valueAddress = nullptr;

            if (material->type == MaterialType::RayTracing)
				valueAddress = GetRTMaterialPropertyAddress(material->data, name, idx);
            else
                valueAddress = GetShaderPropertyAddress(material->shader->reference, material->data->GetID(), name, idx);

            if (valueAddress != nullptr)
                memcpy(valueAddress, array, sizeof(float) * 2);
        }
        delete[] array;
    }

    // Vector3
    void RenderAPIVulkan::SetShaderVector(Material* material, const string& name, const Vector3& value, bool allBuffer)
    {
        SetShaderVector(material, name, value, 0, allBuffer);
    }
    void RenderAPIVulkan::SetShaderVector(Material* material, const string& name, const Vector3& value, uint32_t idx, bool allBuffer)
    {
        float* array = new float[3];
        value.ToArray(array);
        if (allBuffer)
        {
            vector<void*> valueAddresses;

            if (material->type == MaterialType::RayTracing)
                valueAddresses = GetRTMaterialPropertyAddressAllBuffer(material->data, name, idx);
            else
                valueAddresses = GetShaderPropertyAddressAllBuffer(material->shader->reference, material->data->GetID(), name, idx);

            for (auto valueAddress : valueAddresses)
                memcpy(valueAddress, array, sizeof(float) * 3);
        }
        else
        {
            void* valueAddress = nullptr;

            if (material->type == MaterialType::RayTracing)
                valueAddress = GetRTMaterialPropertyAddress(material->data, name, idx);
            else
                valueAddress = GetShaderPropertyAddress(material->shader->reference, material->data->GetID(), name, idx);

            if (valueAddress != nullptr)
                memcpy(valueAddress, array, sizeof(float) * 3);
        }
        delete[] array;
    }

    // Vector4
    void RenderAPIVulkan::SetShaderVector(Material* material, const string& name, const Vector4& value, bool allBuffer)
    {
        SetShaderVector(material, name, value, 0, allBuffer);
    }
    void RenderAPIVulkan::SetShaderVector(Material* material, const string& name, const Vector4& value, uint32_t idx, bool allBuffer)
    {
        float* array = new float[4];
        value.ToArray(array);
        if (allBuffer)
        {
            vector<void*> valueAddresses;

            if (material->type == MaterialType::RayTracing)
                valueAddresses = GetRTMaterialPropertyAddressAllBuffer(material->data, name, idx);
            else
                valueAddresses = GetShaderPropertyAddressAllBuffer(material->shader->reference, material->data->GetID(), name, idx);

            for (auto valueAddress : valueAddresses)
                memcpy(valueAddress, array, sizeof(float) * 4);
        }
        else
        {
            void* valueAddress = nullptr;

            if (material->type == MaterialType::RayTracing)
                valueAddress = GetRTMaterialPropertyAddress(material->data, name, idx);
            else
                valueAddress = GetShaderPropertyAddress(material->shader->reference, material->data->GetID(), name, idx);

            if (valueAddress != nullptr)
                memcpy(valueAddress, array, sizeof(float) * 4);
        }
        delete[] array;
    }

    // Matrix3
    void RenderAPIVulkan::SetShaderMatrix(Material* material, const string& name, const Matrix3& value, bool allBuffer)
    {
        SetShaderMatrix(material, name, value, 0, allBuffer);
    }
    void RenderAPIVulkan::SetShaderMatrix(Material* material, const string& name, const Matrix3& value, uint32_t idx, bool allBuffer)
    {
        float* array = new float[9];
        value.ToColumnMajorArray(array);
        if (allBuffer)
        {
            vector<void*> valueAddresses;

            if (material->type == MaterialType::RayTracing)
                valueAddresses = GetRTMaterialPropertyAddressAllBuffer(material->data, name, idx);
            else
                valueAddresses = GetShaderPropertyAddressAllBuffer(material->shader->reference, material->data->GetID(), name, idx);

            for (auto valueAddress : valueAddresses)
                memcpy(valueAddress, array, sizeof(float) * 9);
        }
        else
        {
            void* valueAddress = nullptr;

            if (material->type == MaterialType::RayTracing)
                valueAddress = GetRTMaterialPropertyAddress(material->data, name, idx);
            else
                valueAddress = GetShaderPropertyAddress(material->shader->reference, material->data->GetID(), name, idx);

            if (valueAddress != nullptr)
                memcpy(valueAddress, array, sizeof(float) * 9);
        }
        delete[] array;
    }

    // Matrix4
    void RenderAPIVulkan::SetShaderMatrix(Material* material, const string& name, const Matrix4& value, bool allBuffer)
    {
        SetShaderMatrix(material, name, value, 0, allBuffer);
    }
    void RenderAPIVulkan::SetShaderMatrix(Material* material, const string& name, const Matrix4& value, uint32_t idx, bool allBuffer)
    {
        float* array = new float[16];
        value.ToColumnMajorArray(array);
        if (allBuffer)
        {
            vector<void*> valueAddresses;

            if (material->type == MaterialType::RayTracing)
                valueAddresses = GetRTMaterialPropertyAddressAllBuffer(material->data, name, idx);
            else
                valueAddresses = GetShaderPropertyAddressAllBuffer(material->shader->reference, material->data->GetID(), name, idx);

            for (auto valueAddress : valueAddresses)
                memcpy(valueAddress, array, sizeof(float) * 16);
        }
        else
        {
            void* valueAddress = nullptr;

            if (material->type == MaterialType::RayTracing)
                valueAddress = GetRTMaterialPropertyAddress(material->data, name, idx);
            else
                valueAddress = GetShaderPropertyAddress(material->shader->reference, material->data->GetID(), name, idx);

            if (valueAddress != nullptr)
                memcpy(valueAddress, array, sizeof(float) * 16);
        }
        delete[] array;
    }

    // Vulkan不需要第4个参数
    void RenderAPIVulkan::SetShaderTexture(Material* material, const string& name, uint32_t ID, uint32_t idx, bool allBuffer, bool isBuffer)
    {
        auto vulkanMaterialData = GetMaterialDataByIndex(material->data->GetID());

        if (allBuffer)
        {
            for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
            {
                vector<VkWriteDescriptorSet> writeDescriptorSets;

                uint32_t textureID = ID;
                if (isBuffer)
                    textureID = GetAttachmentBufferByIndex(ID)->attachmentBuffers[i];

                auto texture = GetTextureByIndex(textureID);
                uint32_t binding = UINT32_MAX;

                for (auto& textureProperty : material->shader->reference->shaderInfo.vertProperties.textureProperties)
                    if (name == textureProperty.name)
                        binding = textureProperty.binding;

                // 没找到的话继续
                if (binding == UINT32_MAX)
                    for (auto& textureProperty : material->shader->reference->shaderInfo.fragProperties.textureProperties)
                        if (name == textureProperty.name)
                            binding = textureProperty.binding;

                if (binding == UINT32_MAX)
                {
                    Debug::LogError("No texture found named: " + name);
                    return;
                }

                VkDescriptorImageInfo imageInfo{};
                imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                imageInfo.imageView = texture->imageView;
                imageInfo.sampler = texture->sampler;
                VkWriteDescriptorSet writeDescriptorSet = {};
                writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                writeDescriptorSet.dstSet = vulkanMaterialData->descriptorSets[i];
                writeDescriptorSet.dstBinding = binding;
                writeDescriptorSet.dstArrayElement = 0;
                writeDescriptorSet.descriptorCount = 1;
                writeDescriptorSet.pImageInfo = &imageInfo;

                writeDescriptorSets.push_back(writeDescriptorSet);

                vkUpdateDescriptorSets(device, static_cast<uint32_t>(writeDescriptorSets.size()), writeDescriptorSets.data(), 0, nullptr);
            }
        }
        else
        {
            vector<VkWriteDescriptorSet> writeDescriptorSets;

            uint32_t textureID = ID;
            if (isBuffer)
                textureID = GetAttachmentBufferByIndex(ID)->attachmentBuffers[currentFrame];

            auto texture = GetTextureByIndex(textureID);
            uint32_t binding = UINT32_MAX;

            for (auto& textureProperty : material->shader->reference->shaderInfo.vertProperties.textureProperties)
                if (name == textureProperty.name)
                    binding = textureProperty.binding;

            // 没找到的话继续
            if (binding == UINT32_MAX)
                for (auto& textureProperty : material->shader->reference->shaderInfo.fragProperties.textureProperties)
                    if (name == textureProperty.name)
                        binding = textureProperty.binding;

            if (binding == UINT32_MAX)
            {
                Debug::LogError("No texture found named: " + name);
                return;
            }

            VkDescriptorImageInfo imageInfo{};
            imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            imageInfo.imageView = texture->imageView;
            imageInfo.sampler = texture->sampler;
            VkWriteDescriptorSet writeDescriptorSet = {};
            writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            writeDescriptorSet.dstSet = vulkanMaterialData->descriptorSets[currentFrame];
            writeDescriptorSet.dstBinding = binding;
            writeDescriptorSet.dstArrayElement = 0;
            writeDescriptorSet.descriptorCount = 1;
            writeDescriptorSet.pImageInfo = &imageInfo;

            writeDescriptorSets.push_back(writeDescriptorSet);

            vkUpdateDescriptorSets(device, static_cast<uint32_t>(writeDescriptorSets.size()), writeDescriptorSets.data(), 0, nullptr);
        }
    }

    void RenderAPIVulkan::SetShaderCubeMap(Material* material, const string& name, uint32_t ID, uint32_t idx, bool allBuffer, bool isBuffer)
    {
        SetShaderTexture(material, name, ID, idx, allBuffer, isBuffer);
    }

    uint32_t RenderAPIVulkan::GetNextVAOIndex()
    {
        uint32_t length = (uint32_t)VulkanVAOArray.size();
        
        for (uint32_t i = 0; i < length; i++)
        {
            if (!VulkanVAOArray[i]->inUse)
                return i;
        }

        VulkanVAOArray.push_back(new VulkanVAO());

        return length;
    }

    VulkanVAO* RenderAPIVulkan::GetVAOByIndex(uint32_t idx)
    {
        return VulkanVAOArray[idx];
    }

    void RenderAPIVulkan::DestroyVAOByIndex(uint32_t idx)
    {
        auto meshBuffer = GetVAOByIndex(idx);

        if (meshBuffer->indexBufferAddress != nullptr)
        {
            vmaUnmapMemory(vmaAllocator, meshBuffer->indexBufferAlloc);
            meshBuffer->indexBufferAddress = nullptr;
        }
        vmaDestroyBuffer(vmaAllocator, meshBuffer->indexBuffer, meshBuffer->indexBufferAlloc);

        if (meshBuffer->vertexBufferAddress != nullptr)
        {
            vmaUnmapMemory(vmaAllocator, meshBuffer->vertexBufferAlloc);
            meshBuffer->vertexBufferAddress = nullptr;
        }
        vmaDestroyBuffer(vmaAllocator, meshBuffer->vertexBuffer, meshBuffer->vertexBufferAlloc);

        if (meshBuffer->blas.isBuilt)
        {
            DestroyAccelerationStructure(meshBuffer->blas);
        }
    
        meshBuffer->inUse = false;
    }

    uint32_t RenderAPIVulkan::GetNextFBOIndex()
    {
        uint32_t length = (uint32_t)VulkanFBOArray.size();

        for (uint32_t i = 0; i < length; i++)
        {
            if (!VulkanFBOArray[i]->inUse)
                return i;
        }

        auto newFBO = new VulkanFBO();
        newFBO->frameBuffers.resize(MAX_FRAMES_IN_FLIGHT);
        VulkanFBOArray.push_back(newFBO);

        return length;
    }

    VulkanFBO* RenderAPIVulkan::GetFBOByIndex(uint32_t idx)
    {
        return VulkanFBOArray[idx];
    }

    void RenderAPIVulkan::DestroyFBOByIndex(uint32_t idx)
    {
        auto vulkanFBO = VulkanFBOArray[idx];

        for (auto iter : vulkanFBO->frameBuffers)
        {
			vkDestroyFramebuffer(device, iter, VK_NULL_HANDLE);
		}
        vulkanFBO->frameBuffers.clear();
        vulkanFBO->frameBuffers.resize(MAX_FRAMES_IN_FLIGHT);

        if (vulkanFBO->colorAttachmentIdx != UINT32_MAX)
        {
            DestroyAttachmentBufferByIndex(vulkanFBO->colorAttachmentIdx);
            vulkanFBO->colorAttachmentIdx = UINT32_MAX;
        }
        if (vulkanFBO->depthAttachmentIdx != UINT32_MAX)
        {
			DestroyAttachmentBufferByIndex(vulkanFBO->depthAttachmentIdx);
			vulkanFBO->depthAttachmentIdx = UINT32_MAX;
		}

        vulkanFBO->bufferType = FrameBufferType::Normal;
        vulkanFBO->renderPassType = RenderPassType::Normal;
        vulkanFBO->clearInfo = {};

        vulkanFBO->inUse = false;
    }

    uint32_t RenderAPIVulkan::GetNextAttachmentBufferIndex()
    {
        uint32_t length = (uint32_t)VulkanAttachmentBufferArray.size();

        for (uint32_t i = 0; i < length; i++)
        {
            if (!VulkanAttachmentBufferArray[i]->inUse)
                return i;
        }

        auto newAttachmentBuffer = new VulkanAttachmentBuffer();
        newAttachmentBuffer->attachmentBuffers.resize(MAX_FRAMES_IN_FLIGHT);
        VulkanAttachmentBufferArray.push_back(newAttachmentBuffer);

        return length;
    }

    VulkanAttachmentBuffer* RenderAPIVulkan::GetAttachmentBufferByIndex(uint32_t idx)
    {
        return VulkanAttachmentBufferArray[idx];
    }

    void RenderAPIVulkan::DestroyAttachmentBufferByIndex(uint32_t idx)
    {
        auto colorAttachmentBuffer = VulkanAttachmentBufferArray[idx];
        for (auto iter : colorAttachmentBuffer->attachmentBuffers)
            DestroyTextureByIndex(iter);
        colorAttachmentBuffer->attachmentBuffers.clear();
        colorAttachmentBuffer->attachmentBuffers.resize(MAX_FRAMES_IN_FLIGHT);
        colorAttachmentBuffer->inUse = false;
    }

    uint32_t RenderAPIVulkan::GetNextDrawCommandIndex()
    {
        uint32_t length = (uint32_t)VulkanDrawCommandArray.size();

        for (uint32_t i = 0; i < length; i++)
        {
            if (!VulkanDrawCommandArray[i]->inUse)
                return i;
        }

        auto newDrawCommand = new VulkanDrawCommand();
        newDrawCommand->drawCommands.resize(MAX_FRAMES_IN_FLIGHT);
        vector<VkCommandBuffer> commandBuffers = {};
        for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
            AllocateCommandBuffer(newDrawCommand->drawCommands[i].commandBuffer);

        VulkanDrawCommandArray.push_back(newDrawCommand);

        return length;
    }

    VulkanDrawCommand* RenderAPIVulkan::GetDrawCommandByIndex(uint32_t idx)
    {
        return VulkanDrawCommandArray[idx];
    }

    uint32_t RenderAPIVulkan::GetNextTextureIndex()
    {
        uint32_t length = (uint32_t)VulkanTextureArray.size();

        for (uint32_t i = 0; i < length; i++)
        {
            if (!VulkanTextureArray[i]->inUse)
                return i;
        }

        VulkanTextureArray.push_back(new VulkanTexture());

        return length;
    }

    VulkanTexture* RenderAPIVulkan::GetTextureByIndex(uint32_t idx)
    {
        return VulkanTextureArray[idx];
    }

    void RenderAPIVulkan::DestroyTextureByIndex(uint32_t idx)
    {
        auto texture = VulkanTextureArray[idx];
        DestroyImageView(texture->imageView);
        DestroyImage(texture->image);
        if (texture->sampler != VK_NULL_HANDLE)
        {
            vkDestroySampler(device, texture->sampler, VK_NULL_HANDLE);
            texture->sampler = VK_NULL_HANDLE;
        }
        texture->inUse = false;

#ifdef ZX_EDITOR
        ImGuiTextureManager::GetInstance()->DeleteByEngineID(idx);
#endif
    }

    uint32_t RenderAPIVulkan::GetNextPipelineIndex()
    {
        uint32_t length = (uint32_t)VulkanPipelineArray.size();

        for (uint32_t i = 0; i < length; i++)
        {
            if (!VulkanPipelineArray[i]->inUse)
                return i;
        }

        VulkanPipelineArray.push_back(new VulkanPipeline());

        return length;
    }

    VulkanPipeline* RenderAPIVulkan::GetPipelineByIndex(uint32_t idx)
    {
        return VulkanPipelineArray[idx];
    }

    void RenderAPIVulkan::DestroyPipelineByIndex(uint32_t idx)
    {
        auto pipeline = GetPipelineByIndex(idx);

        vkDestroyDescriptorSetLayout(device, pipeline->descriptorSetLayout, VK_NULL_HANDLE);
        vkDestroyPipeline(device, pipeline->pipeline, VK_NULL_HANDLE);
        vkDestroyPipelineLayout(device, pipeline->pipelineLayout, VK_NULL_HANDLE);

        if (pipeline->sceneDescriptorSetLayout != VK_NULL_HANDLE)
        {
            vkDestroyDescriptorSetLayout(device, pipeline->sceneDescriptorSetLayout, VK_NULL_HANDLE);
            pipeline->sceneDescriptorSetLayout = VK_NULL_HANDLE;
        }

        pipeline->inUse = false;
    }

    uint32_t RenderAPIVulkan::GetNextMaterialDataIndex()
    {
        uint32_t length = (uint32_t)VulkanMaterialDataArray.size();

        for (uint32_t i = 0; i < length; i++)
        {
            if (!VulkanMaterialDataArray[i]->inUse)
                return i;
        }

        VulkanMaterialDataArray.push_back(new VulkanMaterialData());

        return length;
    }

    VulkanMaterialData* RenderAPIVulkan::GetMaterialDataByIndex(uint32_t idx)
    {
        return VulkanMaterialDataArray[idx];
    }

    void RenderAPIVulkan::DestroyMaterialDataByIndex(uint32_t idx)
    {
        auto vulkanMaterialData = GetMaterialDataByIndex(idx);

        vkDestroyDescriptorPool(device, vulkanMaterialData->descriptorPool, VK_NULL_HANDLE);
        vulkanMaterialData->descriptorSets.clear();

        for (auto& uniformBuffer : vulkanMaterialData->vertUniformBuffers)
            DestroyUniformBuffer(uniformBuffer);
        for (auto& uniformBuffer : vulkanMaterialData->geomUniformBuffers)
            DestroyUniformBuffer(uniformBuffer);
        for (auto& uniformBuffer : vulkanMaterialData->fragUniformBuffers)
            DestroyUniformBuffer(uniformBuffer);

        vulkanMaterialData->vertUniformBuffers.clear();
        vulkanMaterialData->geomUniformBuffers.clear();
        vulkanMaterialData->fragUniformBuffers.clear();

        vulkanMaterialData->inUse = false;
    }

    void* RenderAPIVulkan::GetShaderPropertyAddress(ShaderReference* reference, uint32_t materialDataID, const string& name, uint32_t idx)
    {
        auto vulkanMaterialData = GetMaterialDataByIndex(materialDataID);

        for (auto& property : reference->shaderInfo.vertProperties.baseProperties)
            if (name == property.name)
                return reinterpret_cast<void*>(reinterpret_cast<char*>(vulkanMaterialData->vertUniformBuffers[currentFrame].mappedAddress) + property.offset + property.arrayOffset * idx);

        for (auto& property : reference->shaderInfo.geomProperties.baseProperties)
            if (name == property.name)
                return reinterpret_cast<void*>(reinterpret_cast<char*>(vulkanMaterialData->geomUniformBuffers[currentFrame].mappedAddress) + property.offset + property.arrayOffset * idx);

        for (auto& property : reference->shaderInfo.fragProperties.baseProperties)
            if (name == property.name)
                return reinterpret_cast<void*>(reinterpret_cast<char*>(vulkanMaterialData->fragUniformBuffers[currentFrame].mappedAddress) + property.offset + property.arrayOffset * idx);

        Debug::LogError("Could not find shader property named " + name);

        return nullptr;
    }

    vector<void*> RenderAPIVulkan::GetShaderPropertyAddressAllBuffer(ShaderReference* reference, uint32_t materialDataID, const string& name, uint32_t idx)
    {
        vector<void*> addresses;
        auto vulkanMaterialData = GetMaterialDataByIndex(materialDataID);

        for (auto& property : reference->shaderInfo.vertProperties.baseProperties)
        {
            if (name == property.name)
            {
                uint32_t addressOffset = property.offset + property.arrayOffset * idx;
                for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
                    addresses.push_back(reinterpret_cast<void*>(reinterpret_cast<char*>(vulkanMaterialData->vertUniformBuffers[i].mappedAddress) + addressOffset));
                return addresses;
            }
        }

        for (auto& property : reference->shaderInfo.geomProperties.baseProperties)
        {
            if (name == property.name)
            {
                uint32_t addressOffset = property.offset + property.arrayOffset * idx;
                for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
                    addresses.push_back(reinterpret_cast<void*>(reinterpret_cast<char*>(vulkanMaterialData->geomUniformBuffers[i].mappedAddress) + addressOffset));
                return addresses;
            }
        }

        for (auto& property : reference->shaderInfo.fragProperties.baseProperties)
        {
            if (name == property.name)
            {
                uint32_t addressOffset = property.offset + property.arrayOffset * idx;
                for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
                    addresses.push_back(reinterpret_cast<void*>(reinterpret_cast<char*>(vulkanMaterialData->fragUniformBuffers[i].mappedAddress) + addressOffset));
                return addresses;
            }
        }

        Debug::LogError("Could not find shader property named " + name);

        return addresses;
    }

    void* RenderAPIVulkan::GetRTMaterialPropertyAddress(MaterialData* materialData, const string& name, uint32_t idx)
    {
        auto vulkanRTMaterialData = GetRTMaterialDataByIndex(materialData->GetRTID());

        for (auto& property : materialData->rtMaterialProperties)
            if (name == property.name)
				return reinterpret_cast<void*>(reinterpret_cast<char*>(vulkanRTMaterialData->buffers[currentFrame].mappedAddress) + property.offset + property.arrayOffset * idx);

        Debug::LogError("Could not find ray tracing material property named " + name);

        return nullptr;
    }

    vector<void*> RenderAPIVulkan::GetRTMaterialPropertyAddressAllBuffer(MaterialData* materialData, const string& name, uint32_t idx)
    {
        vector<void*> addresses;
        auto vulkanRTMaterialData = GetRTMaterialDataByIndex(materialData->GetRTID());

        for (auto& property : materialData->rtMaterialProperties)
        {
            if (name == property.name)
            {
				uint32_t addressOffset = property.offset + property.arrayOffset * idx;
				for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
					addresses.push_back(reinterpret_cast<void*>(reinterpret_cast<char*>(vulkanRTMaterialData->buffers[i].mappedAddress) + addressOffset));
				return addresses;
			}
        }

        Debug::LogError("Could not find ray tracing material property named " + name);

        return addresses;
    }


    // ------------------------------------------建立各种Vulkan对象--------------------------------------------


    void RenderAPIVulkan::CreateVkInstance()
    {
        // 初始化volk，用来加载Vulkan Extension函数指针的，类似OpenGL的GLAD，没有这个用不了光追管线的那些Vulkan扩展函数
        VkResult res = volkInitialize();
        if (res != VK_SUCCESS)
			Debug::LogError("Could not initialize volk!");

        if (ProjectSetting::enableGraphicsDebug)
            validationLayersEnabled = CheckValidationLayerSupport();

        // 这个数据可以不填，好像这个数据可以让开发驱动的硬件厂商，比如Nvidia什么的识别一下，给一些引擎或者游戏走后门做特殊处理什么的
        VkApplicationInfo appInfo = {};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "ZXEngineApplication";
        appInfo.applicationVersion = VK_API_VERSION_1_3;
        appInfo.pEngineName = "ZXEngine";
        appInfo.engineVersion = VK_API_VERSION_1_3;
        appInfo.apiVersion = VK_API_VERSION_1_3;

        VkInstanceCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;

        // 获取与不同平台的窗体系统进行交互的扩展信息
        auto extensions = GetRequiredExtensions();
        createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
        createInfo.ppEnabledExtensionNames = extensions.data();

        // 如果开了验证层就添加一下
        if (validationLayersEnabled)
        {
            createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
            createInfo.ppEnabledLayerNames = validationLayers.data();
        }
        else
        {
            createInfo.enabledLayerCount = 0;
        }

        if (vkCreateInstance(&createInfo, nullptr, &vkInstance) != VK_SUCCESS)
            throw std::runtime_error("failed to create instance!");

        // 用volk加载Vulkan Instance级别的函数指针
        volkLoadInstanceOnly(vkInstance);

        // 如果开了验证层，创建一下接收Debug信息的回调
        if (validationLayersEnabled)
            CreateDebugMessenger();
    }

    void RenderAPIVulkan::CreateDebugMessenger()
    {
        VkDebugUtilsMessengerCreateInfoEXT createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        createInfo.pfnUserCallback = VkDebugCallback;

        if (CreateDebugUtilsMessengerEXT(&createInfo, nullptr, &debugMessenger) != VK_SUCCESS)
            throw std::runtime_error("failed to set up debug messenger!");
    }

    void RenderAPIVulkan::CreatePhysicalDevice()
    {
        // 获取当前设备支持Vulkan的显卡数量
        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(vkInstance, &deviceCount, nullptr);
        if (deviceCount == 0)
            throw std::runtime_error("failed to find GPUs with Vulkan support!");

        // 获取所有支持Vulkan的显卡，存放到devices里
        vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(vkInstance, &deviceCount, devices.data());

        // 遍历所有显卡，找到第一个符合我们需求的
        // 其实可以写个多线程同时调用这些显卡，不过现在先随便用一个
        for (const auto& device : devices)
        {
            if (IsPhysicalDeviceSuitable(device))
            {
                physicalDevice = device;
                break;
            }
        }

        if (physicalDevice == VK_NULL_HANDLE)
            throw std::runtime_error("failed to find a suitable GPU!");

        GetPhysicalDeviceProperties();
    }

    void RenderAPIVulkan::CreateLogicalDevice()
    {
        // 获取当前物理设备的队列簇索引
        queueFamilyIndices = GetQueueFamilyIndices(physicalDevice);

        // 逻辑设备需要哪些Queue
        vector<VkDeviceQueueCreateInfo> queueCreateInfos;
        float queuePriority = 1.0f;
        set<uint32_t> uniqueQueueFamilies = { queueFamilyIndices.graphics, queueFamilyIndices.present };
        // 有多个队列簇，遍历创建
        for (uint32_t queueFamily : uniqueQueueFamilies)
        {
            VkDeviceQueueCreateInfo queueCreateInfo = {};
            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            // 当前队列簇对应的索引
            queueCreateInfo.queueFamilyIndex = queueFamily;
            // 队列簇中需要申请使用的队列数量
            queueCreateInfo.queueCount = 1;
            // Vulkan允许使用0.0到1.0之间的浮点数分配队列优先级来影响命令缓冲区执行的调用，即使只有一个队列也是必须的
            queueCreateInfo.pQueuePriorities = &queuePriority;
            queueCreateInfos.push_back(queueCreateInfo);
        }

        // 明确设备要使用的功能特性
        VkPhysicalDeviceFeatures2 deviceFeatures = {};
        deviceFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
        // 启用对各向异性采样的支持
        deviceFeatures.features.samplerAnisotropy = VK_TRUE;
        deviceFeatures.features.geometryShader = VK_TRUE;
        deviceFeatures.features.sampleRateShading = VK_TRUE;
        deviceFeatures.features.shaderInt64 = VK_TRUE;

        // 添加光追管线需要的扩展和特性
        // 对应扩展: VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME
        VkPhysicalDeviceAccelerationStructureFeaturesKHR accelerationFeature = {};
        accelerationFeature.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_FEATURES_KHR;
        accelerationFeature.accelerationStructure = VK_TRUE;
        deviceFeatures.pNext = &accelerationFeature;
        // 对应扩展: VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME
        VkPhysicalDeviceRayTracingPipelineFeaturesKHR rtPipelineFeature = {};
        rtPipelineFeature.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_FEATURES_KHR;
        rtPipelineFeature.rayTracingPipeline = VK_TRUE;
        accelerationFeature.pNext = &rtPipelineFeature;

        // 添加Shader计时器需要的扩展(光追Shader要用)
        // 对应扩展: VK_KHR_SHADER_CLOCK_EXTENSION_NAME
        VkPhysicalDeviceShaderClockFeaturesKHR shaderClockFeature = {};
        shaderClockFeature.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_CLOCK_FEATURES_KHR;
        shaderClockFeature.shaderDeviceClock = VK_TRUE;
        shaderClockFeature.shaderSubgroupClock = VK_TRUE;
        rtPipelineFeature.pNext = &shaderClockFeature;

        // 添加Vulkan 1.2的特性
        VkPhysicalDeviceVulkan12Features deviceVulkan12Features = {};
        deviceVulkan12Features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
        // 启用对Device Address的支持
        deviceVulkan12Features.bufferDeviceAddress = VK_TRUE;
        deviceVulkan12Features.runtimeDescriptorArray = VK_TRUE;
        deviceVulkan12Features.shaderSampledImageArrayNonUniformIndexing = VK_TRUE;
        deviceVulkan12Features.hostQueryReset = VK_TRUE;
        shaderClockFeature.pNext = &deviceVulkan12Features;
        deviceVulkan12Features.pNext = nullptr;

        // 创建逻辑设备的信息
        VkDeviceCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        // 使用前面的两个结构体填充
        createInfo.pQueueCreateInfos = queueCreateInfos.data();
        createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());

        // 添加VkDevice级别的扩展和特性
        createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
        createInfo.ppEnabledExtensionNames = deviceExtensions.data();
        // Vulkan 1.1之后，用这种pNext方式添加特性，而不是pEnabledFeatures
        createInfo.pNext = &deviceFeatures;
        createInfo.pEnabledFeatures = VK_NULL_HANDLE;

        // 添加VkDevice级别验证层
        // 如果启用了验证层，把验证层信息添加进去
        if (validationLayersEnabled)
        {
            createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
            createInfo.ppEnabledLayerNames = validationLayers.data();
        }
        else
        {
            createInfo.enabledLayerCount = 0;
        }

        // 调用vkCreateDevice函数来创建实例化逻辑设备
        // 逻辑设备不与VkInstance直接交互，所以参数里只有物理设备
        if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS)
            throw std::runtime_error("failed to create logical device!");
        
        // 用volk加载Vulkan Device级别的函数指针
        volkLoadDevice(device);

        // 逻辑设备创建的时候，队列也一起创建了，获取队列并保存下来方便之后调用
        // 参数是逻辑设备，队列簇，队列索引和存储获取队列变量句柄的指针
        // 因为我们只是从这个队列簇创建一个队列，所以需要使用索引0
        vkGetDeviceQueue(device, queueFamilyIndices.graphics, 0, &graphicsQueue);
        vkGetDeviceQueue(device, queueFamilyIndices.present, 0, &presentQueue);
    }

    void RenderAPIVulkan::CreateMemoryAllocator()
    {
        // 因为用volk库手动加载所有Vulkan函数了，所以这里要给VMA传递获取函数地址的方法，让VMA可以正确获取Vulkan函数
        VmaVulkanFunctions vmaVkFunctions = {};
        vmaVkFunctions.vkGetInstanceProcAddr = vkGetInstanceProcAddr;
        vmaVkFunctions.vkGetDeviceProcAddr = vkGetDeviceProcAddr;

        VmaAllocatorCreateInfo vmaInfo = {};
        vmaInfo.vulkanApiVersion = VK_HEADER_VERSION_COMPLETE;
        vmaInfo.instance = vkInstance;
        vmaInfo.physicalDevice = physicalDevice;
        vmaInfo.device = device;
        vmaInfo.flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;
        // 如果不手动加载Vulkan函数，这里可以填NULL
        vmaInfo.pVulkanFunctions = &vmaVkFunctions;

        vmaCreateAllocator(&vmaInfo, &vmaAllocator);
    }

    void RenderAPIVulkan::CreateCommandPool()
    {
        VkCommandPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        // 这个flags可要可不要，如果要多个flags的话用|即可
        // VK_COMMAND_POOL_CREATE_TRANSIENT_BIT: 提示命令缓冲区非常频繁的重新记录新命令(可能会改变内存分配行为)
        // VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT: 允许命令缓冲区单独重新记录，没有这个标志，所有的命令缓冲区都必须一起重置
        poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        // command buffer是通过在一个设备队列上提交它们来执行的，每个命令池只能分配在单一类型队列上提交的命令缓冲区
        // 我们将记录用于绘图的命令，所以用graphicsFamily
        poolInfo.queueFamilyIndex = queueFamilyIndices.graphics;

        if (vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool) != VK_SUCCESS)
            throw std::runtime_error("failed to create command pool!");
    }

    void RenderAPIVulkan::CreateSurface() {
        // surface的具体创建过程是要区分平台的，这里直接用GLFW封装好的接口来创建
        if (glfwCreateWindowSurface(vkInstance, static_cast<GLFWwindow*>(WindowManager::GetInstance()->GetWindow()), nullptr, &surface) != VK_SUCCESS)
            throw std::runtime_error("failed to create window surface!");
    }

    void RenderAPIVulkan::CreateSwapChain()
    {
        // 查询硬件支持的交换链设置
        SwapChainSupportDetails swapChainSupport = GetSwapChainSupportDetails(physicalDevice);

        // 选择一个图像格式
        VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(swapChainSupport.formats);
        // 选择一个present模式(就是图像交换的模式)
        VkPresentModeKHR presentMode = ChooseSwapPresentMode(swapChainSupport.presentModes);
        // 选择一个合适的图像分辨率
        VkExtent2D extent = ChooseSwapExtent(swapChainSupport.capabilities);

        // 交换链中的图像数量，可以理解为交换队列的长度。它指定运行时图像的最小数量，我们将尝试大于1的图像数量，以实现三重缓冲。
        uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
        // 如果maxImageCount等于0，就表示没有限制。如果大于0就表示有限制，那么我们最大只能设置到maxImageCount
        if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
            imageCount = swapChainSupport.capabilities.maxImageCount;

        // 创建交换链的结构体
        VkSwapchainCreateInfoKHR createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        // 绑定我们的surface
        createInfo.surface = surface;
        // 把前面获取的数据填上
        createInfo.minImageCount = imageCount;
        createInfo.imageFormat = surfaceFormat.format;
        createInfo.imageColorSpace = surfaceFormat.colorSpace;
        createInfo.presentMode = presentMode;
        createInfo.imageExtent = extent;

        // imageArrayLayers指定每个图像有多少层，一般都是1
        createInfo.imageArrayLayers = 1;
        // 这个字段指定在交换链中对图像进行的具体操作
        // 我们如果直接对它们进行渲染，这意味着它们作为颜色附件
        // 也可以首先将图像渲染为单独的图像，进行后处理操作
        // 在这种情况下可以使用像VK_IMAGE_USAGE_TRANSFER_DST_BIT这样的值，并使用内存操作将渲染的图像传输到交换链图像队列
        createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        // 指定如何处理跨多个队列簇的交换链图像
        uint32_t tmpQueueFamilyIndices[] = { queueFamilyIndices.graphics, queueFamilyIndices.present };
        // 如果graphics队列簇与presentation队列簇不同，会出现如下情形
        // 我们将从graphics队列中绘制交换链的图像，然后在另一个presentation队列中提交他们
        // 多队列处理图像有两种方法
        // VK_SHARING_MODE_EXCLUSIVE: 同一时间图像只能被一个队列簇占用，如果其他队列簇需要其所有权需要明确指定，这种方式提供了最好的性能
        // VK_SHARING_MODE_CONCURRENT: 图像可以被多个队列簇访问，不需要明确所有权从属关系
        // 如果队列簇不同，暂时使用concurrent模式，避免处理图像所有权从属关系的内容，因为这些会涉及不少概念
        if (queueFamilyIndices.graphics != queueFamilyIndices.present)
        {
            createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            // Concurrent模式需要预先指定队列簇所有权从属关系，通过queueFamilyIndexCount和pQueueFamilyIndices参数进行共享
            createInfo.queueFamilyIndexCount = 2;
            createInfo.pQueueFamilyIndices = tmpQueueFamilyIndices;
        }
        // 如果graphics队列簇和presentation队列簇相同，我们需要使用exclusive模式，因为concurrent模式需要至少两个不同的队列簇
        else
        {
            createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
            createInfo.queueFamilyIndexCount = 0; // Optional
            createInfo.pQueueFamilyIndices = nullptr; // Optional
        }

        // 如果交换链支持(supportedTransforms in capabilities)，我们可以为交换链图像指定某些转换逻辑
        // 比如90度顺时针旋转或者水平反转。如果不需要任何transoform操作，可以简单的设置为currentTransoform
        createInfo.preTransform = swapChainSupport.capabilities.currentTransform;

        // compositeAlpha字段指定alpha通道是否应用于与其他的窗体系统进行混合操作。如果忽略该功能，简单的填VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR
        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

        // 如果clipped成员设置为VK_TRUE，意味着我们不关心被遮蔽的像素数据，比如由于其他的窗体置于前方时或者渲染的部分内容存在于可是区域之外
        // 除非真的需要读取这些像素获数据进行处理，否则可以开启裁剪获得最佳性能
        createInfo.clipped = VK_TRUE;

        // Vulkan运行时，交换链可能在某些条件下被替换，比如窗口调整大小或者交换链需要重新分配更大的图像队列
        // 在这种情况下，交换链实际上需要重新分配创建，可以在此字段中指定对旧的引用，用以回收资源
        // 也可以自己手动销毁旧交换链再重新创建，不使用这个参数
        // 但是用这个参数会好一点，好像可以在旧交换链还在展示的时候先绘制后续图像到新交换链，否则需要等旧交换链任务执行完，再销毁，创建新的，再绘制
        createInfo.oldSwapchain = VK_NULL_HANDLE;

        // 创建交换链
        if (vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapChain) != VK_SUCCESS) {
            throw std::runtime_error("failed to create swap chain!");
        }

        // 获取一下交换链里图像的数量，这个imageCount变量最开始是我们期望的图像数量，但是实际创建的不一定是这么多，所以要重新获取一下
        vkGetSwapchainImagesKHR(device, swapChain, &imageCount, nullptr);
        // 根据实际图像数量重新设置vector大小
        swapChainImages.resize(imageCount);
        // 把交换链上的图像存储到swapChainImages中
        vkGetSwapchainImagesKHR(device, swapChain, &imageCount, swapChainImages.data());
        // 存一下交换链图像的格式和分辨率
        swapChainImageFormat = surfaceFormat.format;
        swapChainExtent = extent;
    }

    void RenderAPIVulkan::CreatePresentFrameBuffer()
    {
        presentFBOIdx = GetNextFBOIndex();

        uint32_t colorAttachmentIdx = GetNextAttachmentBufferIndex();
        auto colorAttachmentBuffer = GetAttachmentBufferByIndex(colorAttachmentIdx);
        colorAttachmentBuffer->attachmentBuffers.clear();
        colorAttachmentBuffer->attachmentBuffers.resize(swapChainImages.size());
        colorAttachmentBuffer->inUse = true;

        auto vulkanFBO = GetFBOByIndex(presentFBOIdx);
        vulkanFBO->bufferType = FrameBufferType::Present;
        vulkanFBO->renderPassType = RenderPassType::Present;
        vulkanFBO->colorAttachmentIdx = colorAttachmentIdx;
        vulkanFBO->frameBuffers.clear();
        vulkanFBO->frameBuffers.resize(swapChainImages.size());

        swapChainImageViews.resize(swapChainImages.size());
        for (size_t i = 0; i < swapChainImages.size(); i++)
        {
            swapChainImageViews[i] = CreateImageView(swapChainImages[i], swapChainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_VIEW_TYPE_2D);

            VulkanImage colorImage = CreateImage(swapChainExtent.width, swapChainExtent.height, 1, 1, msaaSamplesCount, swapChainImageFormat, VK_IMAGE_TILING_OPTIMAL,
                VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE);
            VkImageView colorImageView = CreateImageView(colorImage.image, swapChainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_VIEW_TYPE_2D);
            colorAttachmentBuffer->attachmentBuffers[i] = CreateVulkanTexture(colorImage, colorImageView, VK_NULL_HANDLE);

            array<VkImageView, 2> attachments = { colorImageView, swapChainImageViews[i] };

            VkFramebufferCreateInfo framebufferInfo{};
            framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferInfo.renderPass = GetRenderPass(RenderPassType::Present);
            framebufferInfo.pAttachments = attachments.data();
            framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
            framebufferInfo.width = swapChainExtent.width;
            framebufferInfo.height = swapChainExtent.height;
            // layer是指定图像数组中的层数，交换链图像是单个图像，因此层数为1
            framebufferInfo.layers = 1;

            if (vkCreateFramebuffer(device, &framebufferInfo, nullptr, &vulkanFBO->frameBuffers[i]) != VK_SUCCESS)
                throw std::runtime_error("failed to create framebuffer!");
        }

        vulkanFBO->inUse = true;
    }

    void RenderAPIVulkan::DestroyPresentFrameBuffer()
    {
        auto vulkanFBO = GetFBOByIndex(presentFBOIdx);
        DestroyAttachmentBufferByIndex(vulkanFBO->colorAttachmentIdx);

        for (auto iter : vulkanFBO->frameBuffers)
			vkDestroyFramebuffer(device, iter, VK_NULL_HANDLE);
		vulkanFBO->frameBuffers.clear();
        vulkanFBO->frameBuffers.resize(MAX_FRAMES_IN_FLIGHT);

        vulkanFBO->inUse = false;
    }


    // ----------------------------------------建立Vulkan对象的辅助函数----------------------------------------


    bool RenderAPIVulkan::CheckValidationLayerSupport()
    {
        // 获取所有可用的Layer数量
        uint32_t layerCount = 0;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

        // 获取所有可用Layer的属性
        vector<VkLayerProperties> availableLayers(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

        // 检查我们的validationLayers中的所有layer是否存在于availableLayers列表中
        for (const char* layerName : validationLayers)
        {
            bool layerFound = false;
            for (const auto& layerProperties : availableLayers)
            {
                if (strcmp(layerName, layerProperties.layerName) == 0)
                {
                    layerFound = true;
                    break;
                }
            }
            if (!layerFound)
                return false;
        }

        return true;
    }

    vector<const char*> RenderAPIVulkan::GetRequiredExtensions()
    {
        // Vulakn对于平台特性是零API支持的(至少暂时这样)，这意味着需要一个扩展才能与不同平台的窗体系统进行交互
        // GLFW有一个方便的内置函数，返回它有关的扩展信息
        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions;
        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        vector<const char*> extensions;

        // 添加GLFW获取的扩展
        for (unsigned int i = 0; i < glfwExtensionCount; i++)
            extensions.push_back(glfwExtensions[i]);

        // 如果开启了Debug，添加Debug的扩展
        if (validationLayersEnabled)
            extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

        return extensions;
    }

    // Vulkan的Debug是扩展功能，所以vkCreateDebugUtilsMessengerEXT并不会自动加载
    // 想调用它需要通过vkGetInstanceProcAddr手动获取函数地址
    // 这个函数就是封装了一下获取vkCreateDebugUtilsMessengerEXT地址并调用它的过程
    VkResult RenderAPIVulkan::CreateDebugUtilsMessengerEXT(const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger)
    {
        auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(vkInstance, "vkCreateDebugUtilsMessengerEXT");
        if (func != nullptr)
            return func(vkInstance, pCreateInfo, pAllocator, pDebugMessenger);
        else
            return VK_ERROR_EXTENSION_NOT_PRESENT;
    }

    // 如果用上面的函数创建了VkDebugUtilsMessengerEXT变量，需要手动调用vkDestroyDebugUtilsMessengerEXT清理
    void RenderAPIVulkan::DestroyDebugUtilsMessengerEXT(VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator)
    {
        auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(vkInstance, "vkDestroyDebugUtilsMessengerEXT");
        if (func != nullptr)
            func(vkInstance, debugMessenger, pAllocator);
    }

    bool RenderAPIVulkan::IsPhysicalDeviceSuitable(VkPhysicalDevice device)
    {
        // 获得name, type以及Vulkan版本等基本的设备属性
        VkPhysicalDeviceProperties deviceProperties;
        vkGetPhysicalDeviceProperties(device, &deviceProperties);
        // 需要独显
        if (deviceProperties.deviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
            return false;

        // 查询对纹理压缩，64位浮点数和多视图渲染(VR非常有用)等可选功能的支持
        VkPhysicalDeviceFeatures deviceFeatures;
        vkGetPhysicalDeviceFeatures(device, &deviceFeatures);
        // 需要支持几何着色器
        if (!deviceFeatures.geometryShader)
            return false;

        // 需要支持各项异性采样
        if (!deviceFeatures.samplerAnisotropy)
            return false;

        // 查询符合条件的队列簇
        QueueFamilyIndices indices = GetQueueFamilyIndices(device);
        if (!indices.isComplete())
            return false;

        // 检查是否支持所需要的扩展
        if (!CheckDeviceExtensionSupport(device))
            return false;

        // 检查交换链是否完整
        SwapChainSupportDetails swapChainSupport = GetSwapChainSupportDetails(device);
        // 至少支持一个图像格式和一个Present模式
        bool swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
        if (!swapChainAdequate)
            return false;

        // 如果有很多显卡，可以通过给各种硬件特性一个权重，然后优先选择最合适的
        return true;
    }

    QueueFamilyIndices RenderAPIVulkan::GetQueueFamilyIndices(VkPhysicalDevice device)
    {
        QueueFamilyIndices indices;

        // 获取设备拥有的队列簇数量
        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

        // 获取所有队列簇获的属性
        vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

        // 遍历队列簇，获取支持我们需求的队列簇
        uint32_t i = 0;
        for (const auto& queueFamily : queueFamilies)
        {
            // 当前队列簇是否支持图形处理
            if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
                indices.graphics = i;

            // 是否支持VkSurfaceKHR
            VkBool32 presentSupport = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);
            // 支持的话记录一下索引
            if (queueFamily.queueCount > 0 && presentSupport)
                indices.present = i;

            // 注意这里支持surface和graphic的队列簇不一定是同一个
            // 后续使用这些队列簇的逻辑，要么固定按照支持surface和graphic的队列簇是两个不同的来处理(这样无论是不是同一个都不会出错)
            // 要么这里查找队列簇的时候，明确指定必须同时支持surface和graphic，然后保存同时支持这两个要求的队列簇索引(性能可能会好点)

            if (indices.isComplete())
                break;

            i++;
        }

        // 返回具备图形能力的队列
        return indices;
    }

    bool RenderAPIVulkan::CheckDeviceExtensionSupport(VkPhysicalDevice device) {
        // 获取物理设备支持的扩展数量
        uint32_t extensionCount;
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

        // 获取所支持的具体信息
        vector<VkExtensionProperties> availableExtensions(extensionCount);
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

        // 把我们自定义的deviceExtensions转换成set数据结构(为了避免后面2层for循环的erase，同时也不改动原数据)
        set<string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());
        // 遍历物理设备所支持的扩展，逐个从我们需要的扩展集合中删除
        for (const auto& extension : availableExtensions)
            requiredExtensions.erase(extension.extensionName);

        // 如果全部删除完了，说明我们所需要的扩展都是支持的，否则说明还有一些我们需要的扩展不被支持
        return requiredExtensions.empty();
    }

    SwapChainSupportDetails RenderAPIVulkan::GetSwapChainSupportDetails(VkPhysicalDevice device)
    {
        SwapChainSupportDetails details;

        // 根据VkPhysicalDevice和VkSurfaceKHR获取所支持的具体功能，这两个对象是交换链的核心组件
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

        // 查询支持的surface格式有哪些
        uint32_t formatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);
        if (formatCount != 0)
        {
            // 填充格式数据
            details.formats.resize(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
        }

        // 查询支持的surface模式有哪些
        uint32_t presentModeCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);
        if (presentModeCount != 0) {
            details.presentModes.resize(presentModeCount);
            // 填充模式数据
            vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
        }

        return details;
    }

    void RenderAPIVulkan::GetPhysicalDeviceProperties()
    {
        // 物理设备的光线追踪属性
        rtPhysicalProperties = {};
        rtPhysicalProperties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_PROPERTIES_KHR;

        physicalAccelerationStructureProperties = {};
        physicalAccelerationStructureProperties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_PROPERTIES_KHR;
        rtPhysicalProperties.pNext = &physicalAccelerationStructureProperties;

        VkPhysicalDeviceProperties2 physicalDeviceProperties = {};
        physicalDeviceProperties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
        physicalDeviceProperties.pNext = &rtPhysicalProperties;
        vkGetPhysicalDeviceProperties2(physicalDevice, &physicalDeviceProperties);

        // 取同时支持Color和Depth的最大数量
        VkSampleCountFlags counts = physicalDeviceProperties.properties.limits.framebufferColorSampleCounts & physicalDeviceProperties.properties.limits.framebufferDepthSampleCounts;
        if (counts & VK_SAMPLE_COUNT_64_BIT) { msaaSamplesCount = VK_SAMPLE_COUNT_64_BIT; }
        else if (counts & VK_SAMPLE_COUNT_32_BIT) { msaaSamplesCount = VK_SAMPLE_COUNT_32_BIT; }
        else if (counts & VK_SAMPLE_COUNT_16_BIT) { msaaSamplesCount = VK_SAMPLE_COUNT_16_BIT; }
        else if (counts & VK_SAMPLE_COUNT_8_BIT) { msaaSamplesCount = VK_SAMPLE_COUNT_8_BIT; }
        else if (counts & VK_SAMPLE_COUNT_4_BIT) { msaaSamplesCount = VK_SAMPLE_COUNT_4_BIT; }
        else if (counts & VK_SAMPLE_COUNT_2_BIT) { msaaSamplesCount = VK_SAMPLE_COUNT_2_BIT; }
        else { msaaSamplesCount = VK_SAMPLE_COUNT_1_BIT; }

        maxSamplerAnisotropy = physicalDeviceProperties.properties.limits.maxSamplerAnisotropy;
        minUniformBufferOffsetAlignment = physicalDeviceProperties.properties.limits.minUniformBufferOffsetAlignment;
    }

    VkSurfaceFormatKHR RenderAPIVulkan::ChooseSwapSurfaceFormat(const vector<VkSurfaceFormatKHR>& availableFormats)
    {
        // 查一下有没有我们理想的格式和色彩空间组合，优先用这个
        for (const auto& availableFormat : availableFormats)
        {
            if (availableFormat.format == VK_FORMAT_R8G8B8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
                return availableFormat;
        }

        // 如果没有理想的格式，我们直接选择第一个格式
        // 其实这个时候我们也可以遍历一下availableFormats，自己写一个规则挑一个相对较好的出来
        return availableFormats[0];
    }

    VkPresentModeKHR RenderAPIVulkan::ChooseSwapPresentMode(const vector<VkPresentModeKHR> availablePresentModes) {
        // 在Vulkan中有四个模式可以使用:
        // 1，VK_PRESENT_MODE_IMMEDIATE_KHR
        // 应用程序提交的图像被立即传输到屏幕呈现，这种模式可能会造成撕裂效果。
        // 2，VK_PRESENT_MODE_FIFO_KHR
        // 交换链被看作一个队列，当显示内容需要刷新的时候，显示设备从队列的前面获取图像，并且程序将渲染完成的图像插入队列的后面。如果队列是满的程序会等待。这种规模与视频游戏的垂直同步很类似。显示设备的刷新时刻被称为“垂直中断”。
        // 3，VK_PRESENT_MODE_FIFO_RELAXED_KHR
        // 该模式与上一个模式略有不同的地方为，如果应用程序存在延迟，即接受最后一个垂直同步信号时队列空了，将不会等待下一个垂直同步信号，而是将图像直接传送。这样做可能导致可见的撕裂效果。
        // 4，VK_PRESENT_MODE_MAILBOX_KHR
        // 这是第二种模式的变种。当交换链队列满的时候，选择新的替换旧的图像，从而替代阻塞应用程序的情形。这种模式通常用来实现三重缓冲区，与标准的垂直同步双缓冲相比，它可以有效避免延迟带来的撕裂效果。

        // 默认的模式
        VkPresentModeKHR bestMode = VK_PRESENT_MODE_FIFO_KHR;

        for (const auto& availablePresentMode : availablePresentModes)
        {
            if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
                return availablePresentMode;
            else if (availablePresentMode == VK_PRESENT_MODE_IMMEDIATE_KHR)
                bestMode = availablePresentMode;
        }

        return bestMode;
    }

    VkExtent2D RenderAPIVulkan::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) {
        // 这个capabilities数据是通过vkGetPhysicalDeviceSurfaceCapabilitiesKHR接口查询出来的

        // currentExtent的高宽如果是一个特殊的uint32最大值，就说明允许我们自己在一定范围内自由设置这个值
        if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
        {
            int width, height;
            glfwGetFramebufferSize(static_cast<GLFWwindow*>(WindowManager::GetInstance()->GetWindow()), &width, &height);

            VkExtent2D actualExtent =
            {
                static_cast<uint32_t>(width),
                static_cast<uint32_t>(height)
            };

            // 收敛到minImageExtent和maxImageExtent之间
            actualExtent.width = Math::Max(capabilities.minImageExtent.width, Math::Min(capabilities.maxImageExtent.width, actualExtent.width));
            actualExtent.height = Math::Max(capabilities.minImageExtent.height, Math::Min(capabilities.maxImageExtent.height, actualExtent.height));

            return actualExtent;
        }
        // 否则我们只能使用查询到的currentExtent
        else
        {
            return capabilities.currentExtent;
        }
    }

    void RenderAPIVulkan::RecreateSwapChain()
    {
        int width = 0, height = 0;
        glfwGetFramebufferSize(static_cast<GLFWwindow*>(WindowManager::GetInstance()->GetWindow()), &width, &height);
        // 如果窗口大小为0(被最小化了)，那么程序就在这里等待，直到窗口重新弹出
        while (width == 0 || height == 0)
        {
            glfwGetFramebufferSize(static_cast<GLFWwindow*>(WindowManager::GetInstance()->GetWindow()), &width, &height);
            glfwWaitEvents();
        }

        // 先等逻辑设备执行完当前的所有指令，不再占用资源
        vkDeviceWaitIdle(device);

#ifdef ZX_EDITOR
        uint32_t hWidth = (newWindowWidth - GlobalData::srcWidth) / 3;
        uint32_t iWidth = newWindowWidth - GlobalData::srcWidth - hWidth;
        uint32_t pHeight = newWindowHeight - GlobalData::srcHeight - ProjectSetting::mainBarHeight;
        ProjectSetting::SetWindowSize(hWidth, pHeight, iWidth);
#else
        GlobalData::srcWidth = newWindowWidth;
        GlobalData::srcHeight = newWindowHeight;
#endif

        // 清理所有交换链相关资源，全部重新创建
        CleanUpSwapChain();
        CreateSwapChain();
        CreatePresentFrameBuffer();

        // 重新创建所有大小和窗口保持一致的FBO
        FBOManager::GetInstance()->RecreateAllFollowWindowFBO();

#ifdef ZX_EDITOR
        EditorGUIManager::GetInstance()->OnWindowSizeChange();
#endif

        windowResized = false;
    }

    void RenderAPIVulkan::CleanUpSwapChain()
    {
        DestroyPresentFrameBuffer();

        for (auto iter : swapChainImageViews)
            vkDestroyImageView(device, iter, VK_NULL_HANDLE);
        swapChainImageViews.clear();

        vkDestroySwapchainKHR(device, swapChain, VK_NULL_HANDLE);
    }


    VulkanBuffer RenderAPIVulkan::CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VmaMemoryUsage memoryUsage, bool cpuAddress, bool gpuAddress)
    {
        if (gpuAddress)
            usage |= VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT;

        VkBufferCreateInfo bufferInfo = {};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = size;
        bufferInfo.usage = usage;

        VmaAllocationCreateInfo allocationInfo = {};
        allocationInfo.usage = memoryUsage;
        if (cpuAddress)
            allocationInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;

        VulkanBuffer newBuffer;
        vmaCreateBuffer(vmaAllocator, &bufferInfo, &allocationInfo, &newBuffer.buffer, &newBuffer.allocation, nullptr);

        if (cpuAddress)
			vmaMapMemory(vmaAllocator, newBuffer.allocation, &newBuffer.mappedAddress);

        if (gpuAddress)
        {
			VkBufferDeviceAddressInfoKHR deviceAddressInfo = {};
            deviceAddressInfo.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
            deviceAddressInfo.buffer = newBuffer.buffer;
			newBuffer.deviceAddress = vkGetBufferDeviceAddress(device, &deviceAddressInfo);
		}

        return newBuffer;
    }

    void RenderAPIVulkan::DestroyBuffer(VulkanBuffer buffer)
    {
        if (buffer.mappedAddress != nullptr)
        {
            vmaUnmapMemory(vmaAllocator, buffer.allocation);
            buffer.mappedAddress = nullptr;
        }
        vmaDestroyBuffer(vmaAllocator, buffer.buffer, buffer.allocation);
    }

    UniformBuffer RenderAPIVulkan::CreateUniformBuffer(const vector<ShaderProperty>& properties)
    {
        UniformBuffer uniformBuffer = {};
        if (properties.empty())
        {
            Debug::LogError("Try to create empty uniform buffer !");
            return uniformBuffer;
        }

        VkDeviceSize bufferSize = static_cast<VkDeviceSize>(properties[properties.size() - 1].offset + properties[properties.size() - 1].size);
        // 让Uniform Buffer的大小和minUniformBufferOffsetAlignment对齐，我不太确定这个步骤是不是必要的
        VkDeviceSize remainder = bufferSize % minUniformBufferOffsetAlignment;
        if (remainder > 0)
            bufferSize = bufferSize - remainder + minUniformBufferOffsetAlignment;

        uniformBuffer.binding = properties[0].binding;
        uniformBuffer.size = bufferSize;
        uniformBuffer.buffer = CreateBuffer(uniformBuffer.size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_AUTO, true);
        vmaMapMemory(vmaAllocator, uniformBuffer.buffer.allocation, &uniformBuffer.mappedAddress);

        return uniformBuffer;
    }

    void RenderAPIVulkan::DestroyUniformBuffer(const UniformBuffer& uniformBuffer)
    {
        vmaUnmapMemory(vmaAllocator, uniformBuffer.buffer.allocation);
        DestroyBuffer(uniformBuffer.buffer);
    }

    void RenderAPIVulkan::AllocateCommandBuffer(VkCommandBuffer& commandBuffer)
    {
        VkCommandBufferAllocateInfo allocInfo = {};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = commandPool;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = 1;

        if (vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer) != VK_SUCCESS)
            throw std::runtime_error("failed to allocate command buffers!");
    }

    void RenderAPIVulkan::AllocateCommandBuffers(vector<VkCommandBuffer>& commandBuffers)
    {
        VkCommandBufferAllocateInfo allocInfo = {};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = commandPool;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

        if (vkAllocateCommandBuffers(device, &allocInfo, commandBuffers.data()) != VK_SUCCESS)
            throw std::runtime_error("failed to allocate command buffers!");
    }

    void RenderAPIVulkan::CreateVkFence(VkFence& fence)
    {
        VkFenceCreateInfo fenceInfo = {};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        // 创建时立刻设置为signaled状态(否则第一次的vkWaitForFences永远等不到结果)
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
        if (vkCreateFence(device, &fenceInfo, nullptr, &fence) != VK_SUCCESS)
            throw std::runtime_error("failed to create fence objects!");
    }

    void RenderAPIVulkan::CreateVkSemaphore(VkSemaphore& semaphore)
    {
        VkSemaphoreCreateInfo semaphoreInfo = {};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        if (vkCreateSemaphore(device, &semaphoreInfo, nullptr, &semaphore) != VK_SUCCESS)
            throw std::runtime_error("failed to create synchronization objects for a frame!");
    }

    VulkanImage RenderAPIVulkan::CreateImage(uint32_t width, uint32_t height, uint32_t mipLevels, uint32_t layers, VkSampleCountFlagBits numSamples, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VmaMemoryUsage memoryUsage)
    {
        VkImageCreateInfo imageInfo{};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.extent.width = width;
        imageInfo.extent.height = height;
        // 纹理第三个维度的像素数量，如果不是3D纹理应该都是1
        imageInfo.extent.depth = 1;
        imageInfo.mipLevels = mipLevels;
        imageInfo.arrayLayers = layers;
        imageInfo.format = format;
        // VK_IMAGE_TILING_LINEAR: texel以行为主序排列为数组
        // VK_IMAGE_TILING_OPTIMAL: texel按照Vulkan的具体实现来定义的一种顺序排列，以实现最佳访问
        // 这个和layout不一样，一旦设置之后是固定的不能改，如果CPU需要读取这个数据，就设置为VK_IMAGE_TILING_LINEAR
        // 如果只是GPU使用，就设置为VK_IMAGE_TILING_OPTIMAL性能更好
        imageInfo.tiling = tiling;
        // 这里只能填VK_IMAGE_LAYOUT_UNDEFINED或者VK_IMAGE_LAYOUT_PREINITIALIZED
        // VK_IMAGE_LAYOUT_UNDEFINED意味着第一次transition数据的时候数据会被丢弃
        // VK_IMAGE_LAYOUT_PREINITIALIZED是第一次transition数据的时候数据会被保留
        // 不是很懂这个什么意思，如果是一个用来从CPU写入数据，然后transfer到其它VkImage的stagingImage，就要用VK_IMAGE_LAYOUT_PREINITIALIZED
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageInfo.usage = usage;
        imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        // 这个只影响当作attachments使用的VkImage(自己创建的frame buffer才支持这个，交换链用的那个默认buffer不支持)
        imageInfo.samples = numSamples;
        // 可以加一些标志，给特殊用途的图像做优化，比如3D的稀疏(sparse)图像
        imageInfo.flags = layers == 6 ? VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT : 0;

        VmaAllocationCreateInfo allocationInfo = {};
        allocationInfo.usage = memoryUsage;

        VulkanImage newImage;
        vmaCreateImage(vmaAllocator, &imageInfo, &allocationInfo, &newImage.image, &newImage.allocation, nullptr);
        return newImage;
    }

    void RenderAPIVulkan::DestroyImage(VulkanImage image)
    {
        vmaDestroyImage(vmaAllocator, image.image, image.allocation);
    }

    void RenderAPIVulkan::GenerateMipMaps(VkImage image, VkFormat format, int32_t width, int32_t height, uint32_t mipLevels)
    {
        // 生成mipmap有2种方式
        // 一种是用一些外部的接口，比如stb_image_resize，去生成每一层的图像数据，然后把每一层都当原始图像那样填入数据
        // 我们这里用另一种方式，用vkCmdBlitImage来处理，这个是用于复制，缩放和filter图像数据的
        // 在一个循环里，把level 0(原图)数据缩小一倍blit到level 1，然后1到2，2到3这样

        // 先检查图像格式是否支持linear blitting
        VkFormatProperties formatProperties;
        vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &formatProperties);
        if (!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT))
            throw std::runtime_error("texture image format does not support linear blitting!");

        ImmediatelyExecute([=](VkCommandBuffer cmd)
        {
            VkImageMemoryBarrier barrier{};
            barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
            barrier.image = image;
            barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            barrier.subresourceRange.baseArrayLayer = 0;
            barrier.subresourceRange.layerCount = 1;
            barrier.subresourceRange.levelCount = 1;

            int32_t mipWidth = width;
            int32_t mipHeight = height;
            // 注意循环是从1开始的
            for (uint32_t i = 1; i < mipLevels; i++)
            {
                // 先把第i-1级(0是原图)的layout转成TRANSFER_SRC_OPTIMAL
                barrier.subresourceRange.baseMipLevel = i - 1;
                // 原layout，其实image一开始创建的时候每一级mipmap都设置为VK_IMAGE_LAYOUT_UNDEFINED了
                // 但是每一级mipmap会先作为目标图像接收数据，再作为原图像向下一级传输数据
                // 所以这里第i-1级mipmap，相当于是这一次Blit操作的原数据，也是这个循环里面第二次被使用(第一次被使用是作为目标图像)
                // 所以这里要从TRANSFER_DST_OPTIMAL转换到TRANSFER_SRC_OPTIMAL
                barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
                barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
                // 这个image的数据写入应该在这个Barrier之前完成
                barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
                // 这个Barrier之后就可以读这个image的数据了
                barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

                vkCmdPipelineBarrier(cmd,
                    // 指定应该在Barrier之前完成的操作，在管线里的哪个stage
                    VK_PIPELINE_STAGE_TRANSFER_BIT,
                    // 指定应该等待Barrier的操作，在管线里的哪个stage
                    VK_PIPELINE_STAGE_TRANSFER_BIT,
                    0, 0, nullptr, 0, nullptr, 1, &barrier
                );

                // 配置Blit操作，整个Blit操作就是把同一个image第i-1级mipmap的数据缩小一半复制到第i级
                VkImageBlit blit{};
                // 操作原图像的(0,0)到(width, height)
                blit.srcOffsets[0] = { 0, 0, 0 };
                blit.srcOffsets[1] = { mipWidth, mipHeight, 1 };
                // 操作原图像的Color
                blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                // 操作原图像mipmap等级的i-1
                blit.srcSubresource.mipLevel = i - 1;
                // 暂时没用
                blit.srcSubresource.baseArrayLayer = 0;
                blit.srcSubresource.layerCount = 1;
                // 复制到目标图像的(0,0)到(width/2, height/2)，如果小于1的话等于1
                blit.dstOffsets[0] = { 0, 0, 0 };
                blit.dstOffsets[1] = { mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1 };
                // 操作目标图像的Color
                blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                // 复制到目标图像的mipmap等级i
                blit.dstSubresource.mipLevel = i;
                // 暂时没用
                blit.dstSubresource.baseArrayLayer = 0;
                blit.dstSubresource.layerCount = 1;

                // 添加Bilt操作指令，这里原图像和目标图像设置为同一个，因为是同一个image的不同mipmap层操作
                vkCmdBlitImage(cmd,
                    image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                    image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                    1, &blit, VK_FILTER_LINEAR
                );

                // Blit完之后，这个Barrier所对应的i-1级mipmap就结束任务了，可以提供给shader读取了
                // 所以layout从TRANSFER_SRC_OPTIMAL转换到SHADER_READ_ONLY_OPTIMAL
                barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
                barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                // 这个image第i-1级mipmap的数据读取操作应该在这个Barrier之前完成
                barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
                barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

                vkCmdPipelineBarrier(cmd,
                    // 结合前面的srcAccessMask
                    // transfer阶段的transfer读取操作应该在这个Barrier之前执行
                    VK_PIPELINE_STAGE_TRANSFER_BIT,
                    // 结合前面的dstAccessMask
                    // fragment shader阶段的shader读取操作应该在这个Barrier之后执行
                    VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                    0, 0, nullptr, 0, nullptr, 1, &barrier
                );

                if (mipWidth > 1) mipWidth /= 2;
                if (mipHeight > 1) mipHeight /= 2;
            }

            // 循环结束后还有最后一级的mipmap需要处理
            barrier.subresourceRange.baseMipLevel = mipLevels - 1;
            // 因为最后一级只接收数据，不需要从它复制数据到其它地方，所以最后的layout就是TRANSFER_DST_OPTIMAL
            barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            // 需要转换成shader读取用的SHADER_READ_ONLY_OPTIMAL
            barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            // 这个Barrier之前需要完成最后一级mipmap的数据写入
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            // shader读取数据需要在这个Barrier之后才能开始
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
            
            vkCmdPipelineBarrier(cmd,
                // 结合前面的srcAccessMask
                // transfer阶段的transfer写入操作应该在这个Barrier之前执行
                VK_PIPELINE_STAGE_TRANSFER_BIT,
                // 结合前面的dstAccessMask
                // fragment shader阶段的读取操作需要在这个Barrier之后才能开始
                VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                0, 0, nullptr, 0, nullptr, 1, &barrier
            );
        });
    }

    VkImageView RenderAPIVulkan::CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, VkImageViewType viewType)
    {
        VkImageViewCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.image = image;
        createInfo.viewType = viewType;
        createInfo.format = format;

        // components字段允许调整颜色通道的最终的映射逻辑
        // 比如，我们可以将所有颜色通道映射为红色通道，以实现单色纹理，我们也可以将通道映射具体的常量数值0和1
        // 这里用默认的
        createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

        // subresourceRangle字段用于描述图像的使用目标是什么，以及可以被访问的有效区域
        // 这个图像用作填充color还是depth stencil等
        createInfo.subresourceRange.aspectMask = aspectFlags;
        // 默认处理所有Mipmap
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = VK_REMAINING_MIP_LEVELS;
        // 默认处理所有Layers
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = VK_REMAINING_ARRAY_LAYERS;

        VkImageView imageView;
        if (vkCreateImageView(device, &createInfo, nullptr, &imageView) != VK_SUCCESS)
            throw std::runtime_error("failed to create image views!");
        return imageView;
    }

    void RenderAPIVulkan::DestroyImageView(VkImageView imageView)
    {
        vkDestroyImageView(device, imageView, nullptr);
    }

    VkSampler RenderAPIVulkan::CreateSampler(uint32_t mipLevels)
    {
        VkSamplerCreateInfo samplerInfo{};
        samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerInfo.magFilter = VK_FILTER_LINEAR;
        samplerInfo.minFilter = VK_FILTER_LINEAR;
        samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        // 开启各向异性filter
        samplerInfo.anisotropyEnable = VK_TRUE;
        samplerInfo.maxAnisotropy = maxSamplerAnisotropy;
        samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        // 这里填false，纹理采样坐标范围就是正常的[0, 1)，如果填true，就会变成[0, texWidth)和[0, texHeight)，绝大部分情况下都是用[0, 1)
        samplerInfo.unnormalizedCoordinates = VK_FALSE;
        // 一般用不到这个，某些场景，比如shadow map的percentage-closer filtering会用到
        samplerInfo.compareEnable = VK_FALSE;
        samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
        // mipmap设置
        samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        samplerInfo.mipLodBias = 0.0f;
        samplerInfo.minLod = 0.0f;
        samplerInfo.maxLod = static_cast<float>(mipLevels);

        VkSampler sampler;
        if (vkCreateSampler(device, &samplerInfo, nullptr, &sampler) != VK_SUCCESS)
            throw std::runtime_error("failed to create texture sampler!");

        return sampler;
    }

    uint32_t RenderAPIVulkan::CreateVulkanTexture(VulkanImage image, VkImageView imageView, VkSampler sampler)
    {
        uint32_t textureID = GetNextTextureIndex();
        auto texture = GetTextureByIndex(textureID);
        texture->inUse = true;
        texture->image = image;
        texture->imageView = imageView;
        texture->sampler = sampler;
        return textureID;
    }

    void RenderAPIVulkan::CreateAllRenderPass()
    {
        allVulkanRenderPass.resize((size_t)RenderPassType::MAX);

        allVulkanRenderPass[(size_t)RenderPassType::Present] = CreateRenderPass(RenderPassType::Present);
        allVulkanRenderPass[(size_t)RenderPassType::Normal] = CreateRenderPass(RenderPassType::Normal);
        allVulkanRenderPass[(size_t)RenderPassType::Color] = CreateRenderPass(RenderPassType::Color);
        allVulkanRenderPass[(size_t)RenderPassType::ShadowCubeMap] = CreateRenderPass(RenderPassType::ShadowCubeMap);
    }

    VkRenderPass RenderAPIVulkan::CreateRenderPass(RenderPassType type)
    {
        VkRenderPass renderPass = {};

        if (type == RenderPassType::Present)
        {
            VkAttachmentDescription colorAttachment = {};
            colorAttachment.format = swapChainImageFormat;
            colorAttachment.samples = msaaSamplesCount;
            colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
            colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            colorAttachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

            VkAttachmentDescription colorAttachmentResolve = {};
            colorAttachmentResolve.format = swapChainImageFormat;
            colorAttachmentResolve.samples = VK_SAMPLE_COUNT_1_BIT;
            colorAttachmentResolve.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            colorAttachmentResolve.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
            colorAttachmentResolve.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            colorAttachmentResolve.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            colorAttachmentResolve.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            colorAttachmentResolve.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

            VkAttachmentReference colorAttachmentRef{};
            colorAttachmentRef.attachment = 0;
            colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

            VkAttachmentReference colorAttachmentResolveRef{};
            colorAttachmentResolveRef.attachment = 1;
            colorAttachmentResolveRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

            VkSubpassDescription subpassInfo = {};
            subpassInfo.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
            subpassInfo.pColorAttachments = &colorAttachmentRef;
            subpassInfo.colorAttachmentCount = 1;
            subpassInfo.pResolveAttachments = &colorAttachmentResolveRef;

            VkSubpassDependency dependency = {};
            dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
            dependency.dstSubpass = 0;
            dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
            dependency.srcAccessMask = 0;
            dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
            dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

            array<VkAttachmentDescription, 2> attachments = { colorAttachment, colorAttachmentResolve };
            VkRenderPassCreateInfo renderPassInfo{};
            renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
            renderPassInfo.pAttachments = attachments.data();
            renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
            renderPassInfo.pSubpasses = &subpassInfo;
            renderPassInfo.subpassCount = 1;
            renderPassInfo.pDependencies = &dependency;
            renderPassInfo.dependencyCount = 1;

            if (vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS)
                throw std::runtime_error("failed to create render pass!");
        }
        else if (type == RenderPassType::Normal)
        {
            VkAttachmentDescription colorAttachment = {};
            colorAttachment.format = defaultImageFormat;
            colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
            colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
            colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
            // 上面那个设置是用于color和depth的，stencil的单独一个
            colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            // 渲染开始前和结束后Layout都是给Shader读取数据准备的，只在渲染中改成Attachment需要的Layout
            colorAttachment.initialLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            colorAttachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

            VkAttachmentDescription depthAttachment = {};
            depthAttachment.format = VK_FORMAT_D16_UNORM;
            depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
            depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
            depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
            depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            depthAttachment.initialLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            depthAttachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

            VkAttachmentReference colorAttachmentRef = {};
            colorAttachmentRef.attachment = 0;
            colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

            VkAttachmentReference depthAttachmentRef = {};
            depthAttachmentRef.attachment = 1;
            depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

            VkSubpassDescription subpassInfo = {};
            subpassInfo.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
            // 指定color buffer的引用，这个引用以及它的索引(上面的attachment = 0)直接对应到片元着色器里的layout(location = 0) out vec4 outColor
            subpassInfo.pColorAttachments = &colorAttachmentRef;
            subpassInfo.colorAttachmentCount = 1;
            subpassInfo.pDepthStencilAttachment = &depthAttachmentRef;

            VkSubpassDependency beginDependency = {};
            beginDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
            beginDependency.dstSubpass = 0;
            beginDependency.srcStageMask = VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT;
            beginDependency.srcAccessMask = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
            beginDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
            beginDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
            VkSubpassDependency endDependency = {};
            endDependency.srcSubpass = 0;
            endDependency.dstSubpass = VK_SUBPASS_EXTERNAL;
            endDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
            endDependency.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
            endDependency.dstStageMask = VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT;
            endDependency.dstAccessMask = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
            array<VkSubpassDependency, 2> dependencies = { beginDependency, endDependency };

            array<VkAttachmentDescription, 2> attachments = { colorAttachment, depthAttachment };
            VkRenderPassCreateInfo renderPassInfo{};
            renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
            // 前面创建VkAttachmentReference的时候，那个索引attachment指的就是在这个pAttachments数组里的索引
            renderPassInfo.pAttachments = attachments.data();
            renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
            renderPassInfo.pSubpasses = &subpassInfo;
            renderPassInfo.subpassCount = 1;
            renderPassInfo.pDependencies = dependencies.data();
            renderPassInfo.dependencyCount = static_cast<uint32_t>(dependencies.size());

            if (vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS)
                throw std::runtime_error("failed to create render pass!");
        }
        else if (type == RenderPassType::Color)
        {
            VkAttachmentDescription colorAttachment = {};
            colorAttachment.format = defaultImageFormat;
            colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
            colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
            colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
            colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            colorAttachment.initialLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            colorAttachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

            VkAttachmentReference colorAttachmentRef = {};
            colorAttachmentRef.attachment = 0;
            colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

            VkSubpassDescription subpassInfo = {};
            subpassInfo.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
            subpassInfo.pColorAttachments = &colorAttachmentRef;
            subpassInfo.colorAttachmentCount = 1;

            VkSubpassDependency beginDependency = {};
            beginDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
            beginDependency.dstSubpass = 0;
            beginDependency.srcStageMask = VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT;
            beginDependency.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
            beginDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
            beginDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
            VkSubpassDependency endDependency = {};
            endDependency.srcSubpass = 0;
            endDependency.dstSubpass = VK_SUBPASS_EXTERNAL;
            endDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
            endDependency.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
            endDependency.dstStageMask = VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT;
            endDependency.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
            array<VkSubpassDependency, 2> dependencies = { beginDependency, endDependency };

            array<VkAttachmentDescription, 1> attachments = { colorAttachment };
            VkRenderPassCreateInfo renderPassInfo{};
            renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
            // 前面创建VkAttachmentReference的时候，那个索引attachment指的就是在这个pAttachments数组里的索引
            renderPassInfo.pAttachments = attachments.data();
            renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
            renderPassInfo.pSubpasses = &subpassInfo;
            renderPassInfo.subpassCount = 1;
            renderPassInfo.pDependencies = dependencies.data();
            renderPassInfo.dependencyCount = static_cast<uint32_t>(dependencies.size());

            if (vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS)
                throw std::runtime_error("failed to create render pass!");
        }
        else if (type == RenderPassType::ShadowCubeMap)
        {
            VkAttachmentDescription depthAttachment = {};
            depthAttachment.format = VK_FORMAT_D16_UNORM;
            depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
            depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
            depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
            depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            depthAttachment.initialLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            depthAttachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

            VkAttachmentReference depthAttachmentRef = {};
            depthAttachmentRef.attachment = 0;
            depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

            VkSubpassDescription subpassInfo = {};
            subpassInfo.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
            subpassInfo.pDepthStencilAttachment = &depthAttachmentRef;

            VkSubpassDependency beginDependency = {};
            beginDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
            beginDependency.dstSubpass = 0;
            beginDependency.srcStageMask = VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT;
            beginDependency.srcAccessMask = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
            beginDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
            beginDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
            VkSubpassDependency endDependency = {};
            endDependency.srcSubpass = 0;
            endDependency.dstSubpass = VK_SUBPASS_EXTERNAL;
            endDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
            endDependency.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
            endDependency.dstStageMask = VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT;
            endDependency.dstAccessMask = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
            array<VkSubpassDependency, 2> dependencies = { beginDependency, endDependency };

            array<VkAttachmentDescription, 1> attachments = { depthAttachment };
            VkRenderPassCreateInfo renderPassInfo{};
            renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
            // 前面创建VkAttachmentReference的时候，那个索引attachment指的就是在这个pAttachments数组里的索引
            renderPassInfo.pAttachments = attachments.data();
            renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
            renderPassInfo.pSubpasses = &subpassInfo;
            renderPassInfo.subpassCount = 1;
            renderPassInfo.pDependencies = dependencies.data();
            renderPassInfo.dependencyCount = static_cast<uint32_t>(dependencies.size());

            if (vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS)
                throw std::runtime_error("failed to create render pass!");
        }
        else
        {
            Debug::LogError("Invalide render pass type.");
        }

        return renderPass;
    }

    VkRenderPass RenderAPIVulkan::GetRenderPass(RenderPassType type)
    {
        return allVulkanRenderPass[(size_t)type];
    }

    void RenderAPIVulkan::DestroyRenderPass(VkRenderPass renderPass)
    {
        vkDestroyRenderPass(device, renderPass, nullptr);
    }

    VkPipeline RenderAPIVulkan::CreatePipeline(const string& path, const ShaderInfo& shaderInfo, VkDescriptorSetLayout& descriptorSetLayout, VkPipelineLayout& pipelineLayout, RenderPassType renderPassType)
    {
        auto shaderModules = CreateShaderModules(path, shaderInfo);
        vector<VkPipelineShaderStageCreateInfo> shaderStages;
        for (auto& shaderModule : shaderModules)
        {
            VkPipelineShaderStageCreateInfo shaderStageInfo = {};
            shaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            shaderStageInfo.stage = shaderModule.first;
            shaderStageInfo.module = shaderModule.second;
            shaderStageInfo.pName = "main";
            shaderStages.push_back(shaderStageInfo);
        }

        // 设置顶点输入格式
        VkVertexInputBindingDescription bindingDescription = {};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(Vertex);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        array<VkVertexInputAttributeDescription, 5> attributeDescriptions = {};
        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(Vertex, Position);
        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(Vertex, TexCoords);
        attributeDescriptions[2].binding = 0;
        attributeDescriptions[2].location = 2;
        attributeDescriptions[2].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[2].offset = offsetof(Vertex, Normal);
        attributeDescriptions[3].binding = 0;
        attributeDescriptions[3].location = 3;
        attributeDescriptions[3].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[3].offset = offsetof(Vertex, Tangent);
        attributeDescriptions[4].binding = 0;
        attributeDescriptions[4].location = 4;
        attributeDescriptions[4].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[4].offset = offsetof(Vertex, Bitangent);
        VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
        vertexInputInfo.vertexBindingDescriptionCount = 1;
        vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();
        vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());

        // 设置图元
        VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo = GetAssemblyInfo(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);

        // ViewPort信息，这里不直接设置，下面弄成动态的
        VkPipelineViewportStateCreateInfo viewportStateInfo = {};
        viewportStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportStateInfo.viewportCount = 1;
        viewportStateInfo.scissorCount = 1;

        // View Port和Scissor设置为动态，每帧绘制时决定
        vector<VkDynamicState> dynamicStates = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
        VkPipelineDynamicStateCreateInfo dynamicStateInfo = {};
        dynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamicStateInfo.pDynamicStates = dynamicStates.data();
        dynamicStateInfo.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());

        // 设置光栅化阶段
        VkPipelineRasterizationStateCreateInfo rasterizationInfo = GetRasterizationInfo(vkFaceCullOptionMap[shaderInfo.stateSet.cull]);
        
        // 设置Shader采样纹理的MSAA(不是输出到屏幕上的MSAA)，需要创建逻辑设备的时候开启VkPhysicalDeviceFeatures里的sampleRateShading才能生效，暂时关闭
        VkPipelineMultisampleStateCreateInfo multisampleInfo = GetPipelineMultisampleInfo(renderPassType == RenderPassType::Present ? msaaSamplesCount : VK_SAMPLE_COUNT_1_BIT);

        // Color Blend
        VkPipelineColorBlendAttachmentState colorBlendAttachment{};
        colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        colorBlendAttachment.blendEnable = VK_TRUE;
        colorBlendAttachment.srcColorBlendFactor = vkBlendFactorMap[shaderInfo.stateSet.srcFactor];
        colorBlendAttachment.dstColorBlendFactor = vkBlendFactorMap[shaderInfo.stateSet.dstFactor];
        colorBlendAttachment.colorBlendOp = vkBlendOptionMap[shaderInfo.stateSet.blendOp];
        colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
        VkPipelineColorBlendStateCreateInfo colorBlending{};
        colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlending.logicOpEnable = VK_FALSE;
        colorBlending.logicOp = VK_LOGIC_OP_COPY;
        colorBlending.pAttachments = &colorBlendAttachment;
        colorBlending.attachmentCount = 1;

        // 深度和模板配置
        VkPipelineDepthStencilStateCreateInfo depthStencilInfo = {};
        depthStencilInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        // Depth
        depthStencilInfo.depthWriteEnable = shaderInfo.stateSet.depthWrite ? VK_TRUE : VK_FALSE;
        depthStencilInfo.depthTestEnable = shaderInfo.stateSet.depthCompareOp == CompareOption::ALWAYS ? VK_FALSE : VK_TRUE;
        depthStencilInfo.depthCompareOp = vkDepthTestOptionMap[shaderInfo.stateSet.depthCompareOp];
        depthStencilInfo.depthBoundsTestEnable = VK_FALSE;
        depthStencilInfo.minDepthBounds = 0.0f;
        depthStencilInfo.maxDepthBounds = 1.0f;
        // Stencil
        depthStencilInfo.stencilTestEnable = VK_FALSE;
        depthStencilInfo.front = {};
        depthStencilInfo.back = {};

        descriptorSetLayout = CreateDescriptorSetLayout(shaderInfo);
        pipelineLayout = CreatePipelineLayout({ descriptorSetLayout }, {});

        VkGraphicsPipelineCreateInfo pipelineInfo{};
        pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineInfo.pStages = shaderStages.data();
        pipelineInfo.stageCount = (uint32_t)shaderStages.size();
        pipelineInfo.pVertexInputState = &vertexInputInfo;
        pipelineInfo.pInputAssemblyState = &inputAssemblyInfo;
        pipelineInfo.pViewportState = &viewportStateInfo;
        pipelineInfo.pDynamicState = &dynamicStateInfo;
        pipelineInfo.pRasterizationState = &rasterizationInfo;
        pipelineInfo.pMultisampleState = &multisampleInfo;
        pipelineInfo.pColorBlendState = &colorBlending;
        pipelineInfo.pDepthStencilState = &depthStencilInfo;
        pipelineInfo.layout = pipelineLayout;
        pipelineInfo.renderPass = GetRenderPass(renderPassType);
        pipelineInfo.subpass = 0;
        pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
        pipelineInfo.basePipelineIndex = -1;

        VkPipeline pipeLine;
        if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeLine) != VK_SUCCESS)
            throw std::runtime_error("failed to create graphics pipeline!");

        DestroyShaderModules(shaderModules);

        return pipeLine;
    }

    VkDescriptorPool RenderAPIVulkan::CreateDescriptorPool(const ShaderInfo& info)
    {
        vector<VkDescriptorPoolSize> poolSizes = {};

        if (!info.vertProperties.baseProperties.empty())
        {
            VkDescriptorPoolSize poolSize = {};
            poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            poolSize.descriptorCount = MAX_FRAMES_IN_FLIGHT;
            poolSizes.push_back(poolSize);
        }
        for (auto& texture : info.vertProperties.textureProperties)
        {
            VkDescriptorPoolSize poolSize = {};
            poolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            poolSize.descriptorCount = MAX_FRAMES_IN_FLIGHT;
            poolSizes.push_back(poolSize);
        }

        if (!info.geomProperties.baseProperties.empty())
        {
            VkDescriptorPoolSize poolSize = {};
            poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            poolSize.descriptorCount = MAX_FRAMES_IN_FLIGHT;
            poolSizes.push_back(poolSize);
        }
        for (auto& texture : info.geomProperties.textureProperties)
        {
            VkDescriptorPoolSize poolSize = {};
            poolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            poolSize.descriptorCount = MAX_FRAMES_IN_FLIGHT;
            poolSizes.push_back(poolSize);
        }

        if (!info.fragProperties.baseProperties.empty())
        {
            VkDescriptorPoolSize poolSize = {};
            poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            poolSize.descriptorCount = MAX_FRAMES_IN_FLIGHT;
            poolSizes.push_back(poolSize);
        }
        for (auto& texture : info.fragProperties.textureProperties)
        {
            VkDescriptorPoolSize poolSize = {};
            poolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            poolSize.descriptorCount = MAX_FRAMES_IN_FLIGHT;
            poolSizes.push_back(poolSize);
        }

        VkDescriptorPool descriptorPool = {};
        VkDescriptorPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.pPoolSizes = poolSizes.data();
        poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
        poolInfo.maxSets = MAX_FRAMES_IN_FLIGHT;
        poolInfo.flags = 0;
        if (vkCreateDescriptorPool(device, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) 
            throw std::runtime_error("failed to create descriptor pool!");

        return descriptorPool;
    }

    vector<VkDescriptorSet> RenderAPIVulkan::CreateDescriptorSets(VkDescriptorPool descriptorPool, const vector<VkDescriptorSetLayout>& descriptorSetLayouts)
    {
        VkDescriptorSetAllocateInfo allocInfo = {};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = descriptorPool;
        allocInfo.pSetLayouts = descriptorSetLayouts.data();
        allocInfo.descriptorSetCount = static_cast<uint32_t>(descriptorSetLayouts.size());

        vector<VkDescriptorSet> descriptorSets;
        descriptorSets.resize(descriptorSetLayouts.size());
        if (vkAllocateDescriptorSets(device, &allocInfo, descriptorSets.data()) != VK_SUCCESS)
            throw std::runtime_error("failed to allocate descriptor sets!");

        return descriptorSets;
    }

    VkShaderModule RenderAPIVulkan::CreateShaderModule(vector<char> code)
    {
        VkShaderModuleCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        // 这里需要确保数据满足uint32_t的对齐要求,存储在vector中，默认分配器已经确保数据满足最差情况下的对齐要求
        createInfo.codeSize = code.size();
        // 转换为Vulkan要求的uint32_t指针
        createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

        VkShaderModule shaderModule;
        if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
            throw std::runtime_error("failed to create shader module!");

        return shaderModule;
    }

    ShaderModuleSet RenderAPIVulkan::CreateShaderModules(const string& path, const ShaderInfo& info)
    {
        string prePath = path.substr(0, path.length() - 9);
        ShaderModuleSet shaderModules;

        auto vertShader = Resources::LoadBinaryFile(prePath + ".vert.spv");
        auto vertModule = CreateShaderModule(vertShader);
        shaderModules.insert(make_pair(VK_SHADER_STAGE_VERTEX_BIT, vertModule));

        auto fragShader = Resources::LoadBinaryFile(prePath + ".frag.spv");
        auto fragModule = CreateShaderModule(fragShader);
        shaderModules.insert(make_pair(VK_SHADER_STAGE_FRAGMENT_BIT, fragModule));

        if (info.stages & ZX_SHADER_STAGE_GEOMETRY_BIT)
        {
            auto geomShader = Resources::LoadBinaryFile(prePath + ".geom.spv");
            auto geomModule = CreateShaderModule(geomShader);
            shaderModules.insert(make_pair(VK_SHADER_STAGE_GEOMETRY_BIT, geomModule));
        }

        return shaderModules;
    }

    void RenderAPIVulkan::DestroyShaderModules(ShaderModuleSet shaderModules)
    {
        for (auto& shaderModule : shaderModules)
            vkDestroyShaderModule(device, shaderModule.second, nullptr);
    }

    void RenderAPIVulkan::CheckDeleteData()
    {
        vector<uint32_t> deleteList = {};

        // 材质数据
        for (auto& iter : materialDatasToDelete)
        {
            // 如果这个材质数据的等待帧数大于0，就减1帧
            if (iter.second > 0)
                iter.second--;
            // 否则就删除
            else
                deleteList.push_back(iter.first);
        }
        for (auto id : deleteList)
        {
            DestroyMaterialDataByIndex(id);
            materialDatasToDelete.erase(id);
        }

        // 光追材质数据
        deleteList.clear();
        for (auto& iter : rtMaterialDatasToDelete)
        {
            if (iter.second > 0)
                iter.second--;
            else
                deleteList.push_back(iter.first);
        }
        for (auto id : deleteList)
        {
            DestroyRTMaterialDataByIndex(id);
            rtMaterialDatasToDelete.erase(id);
        }

        // Texture
        deleteList.clear();
        for (auto& iter : texturesToDelete)
        {
            if (iter.second > 0)
                iter.second--;
            else
                deleteList.push_back(iter.first);
        }
        for (auto id : deleteList)
        {
            DestroyTextureByIndex(id);
            texturesToDelete.erase(id);
        }

        // Mesh
        deleteList.clear();
        for (auto& iter : meshsToDelete)
        {
            if (iter.second > 0)
                iter.second--;
            else
                deleteList.push_back(iter.first);
        }
        for (auto id : deleteList)
        {
            DestroyVAOByIndex(id);
            meshsToDelete.erase(id);
        }

        // Shader
        deleteList.clear();
        for (auto& iter : pipelinesToDelete)
        {
            if (iter.second > 0)
                iter.second--;
            else
                deleteList.push_back(iter.first);
        }
        for (auto id : deleteList)
        {
            DestroyPipelineByIndex(id);
            pipelinesToDelete.erase(id);
        }
    }


    uint32_t RenderAPIVulkan::GetNextTLASGroupIndex()
    {
        uint32_t length = static_cast<uint32_t>(VulkanTLASGroupArray.size());

        for (uint32_t i = 0; i < length; i++)
        {
            if (!VulkanTLASGroupArray[i]->inUse)
                return i;
        }

        VulkanTLASGroupArray.push_back(new VulkanASGroup());

        return length;
    }

    VulkanASGroup* RenderAPIVulkan::GetTLASGroupByIndex(uint32_t idx)
    {
        return VulkanTLASGroupArray[idx];
    }

    void RenderAPIVulkan::DestroyTLASGroupByIndex(uint32_t idx)
    {
        auto vulkanASGroup = GetTLASGroupByIndex(idx);

        for (auto& tlas : vulkanASGroup->asGroup)
            DestroyAccelerationStructure(tlas);

        vulkanASGroup->inUse = false;
    }

    uint32_t RenderAPIVulkan::GetNextRTMaterialDataIndex()
    {
        uint32_t length = static_cast<uint32_t>(VulkanRTMaterialDataArray.size());

        for (uint32_t i = 0; i < length; i++)
        {
            if (!VulkanRTMaterialDataArray[i]->inUse)
                return i;
        }

        VulkanRTMaterialDataArray.push_back(new VulkanRTMaterialData());

        return length;
    }

    VulkanRTMaterialData* RenderAPIVulkan::GetRTMaterialDataByIndex(uint32_t idx)
    {
        return VulkanRTMaterialDataArray[idx];
    }

    void RenderAPIVulkan::DestroyRTMaterialDataByIndex(uint32_t idx)
    {
        auto vulkanRTMaterialData = GetRTMaterialDataByIndex(idx);

        for (auto& buffer : vulkanRTMaterialData->buffers)
            DestroyBuffer(buffer);

        vulkanRTMaterialData->buffers.clear();

        vulkanRTMaterialData->inUse = false;
    }

    void RenderAPIVulkan::DestroyAccelerationStructure(VulkanAccelerationStructure& accelerationStructure)
    {
        accelerationStructure.deviceAddress = 0;
        DestroyBuffer(accelerationStructure.buffer);
        vkDestroyAccelerationStructureKHR(device, accelerationStructure.as, nullptr);
        accelerationStructure.as = VK_NULL_HANDLE;
        accelerationStructure.isBuilt = false;
    }

    void RenderAPIVulkan::CreateRTPipelineData(uint32_t id)
    {
        auto rtPipeline = rtPipelines[id];

        // 创建DescriptorPool
        vector<VkDescriptorPoolSize> poolSizes = {};

        // Top Level Acceleration Structure
        VkDescriptorPoolSize asPoolSize = {};
        asPoolSize.type = VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR;
        asPoolSize.descriptorCount = MAX_FRAMES_IN_FLIGHT;
        poolSizes.push_back(asPoolSize);

        // 输出渲染结果的Storage Image
        VkDescriptorPoolSize imagePoolSize = {};
        imagePoolSize.type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
        imagePoolSize.descriptorCount = MAX_FRAMES_IN_FLIGHT;
        poolSizes.push_back(imagePoolSize);

        VkDescriptorPoolCreateInfo poolInfo = {};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.pPoolSizes = poolSizes.data();
        poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
        poolInfo.maxSets = MAX_FRAMES_IN_FLIGHT;
        poolInfo.flags = 0;
        if (vkCreateDescriptorPool(device, &poolInfo, nullptr, &rtPipeline->pipelineData.descriptorPool) != VK_SUCCESS)
            throw std::runtime_error("Failed to create descriptor pool for ray tracing!");

        // 创建DescriptorSet
        vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, rtPipeline->pipeline.descriptorSetLayout);
        rtPipeline->pipelineData.descriptorSets = CreateDescriptorSets(rtPipeline->pipelineData.descriptorPool, layouts);
    }

    void RenderAPIVulkan::UpdateRTPipelineData(uint32_t id)
    {
        auto rtPipeline = rtPipelines[id];
        auto& curTLAS = GetTLASGroupByIndex(rtPipeline->tlasIdx)->asGroup[currentFrame];

        // 更新TLAS
        VkWriteDescriptorSetAccelerationStructureKHR writeASInfo = {};
        writeASInfo.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET_ACCELERATION_STRUCTURE_KHR;
        writeASInfo.accelerationStructureCount = 1;
        writeASInfo.pAccelerationStructures = &curTLAS.as;

        VkWriteDescriptorSet writeAS = {};
        writeAS.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        writeAS.descriptorType = VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR;
        writeAS.dstSet = rtPipeline->pipelineData.descriptorSets[currentFrame];
        writeAS.dstBinding = 0;
        writeAS.descriptorCount = 1;
        writeAS.pNext = &writeASInfo;

        // 获取光追管线输出的目标图像
        auto curFBO = GetFBOByIndex(curFBOIdx);
        uint32_t textureID = GetAttachmentBufferByIndex(curFBO->colorAttachmentIdx)->attachmentBuffers[currentFrame];
        auto texture = GetTextureByIndex(textureID);

        // 更新输出目标图像
        VkDescriptorImageInfo imageInfo = {};
        imageInfo.imageView = texture->imageView;
        imageInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;

        VkWriteDescriptorSet writeImage = {};
        writeImage.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        writeImage.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
        writeImage.dstSet = rtPipeline->pipelineData.descriptorSets[currentFrame];
        writeImage.dstBinding = 1;
        writeImage.descriptorCount = 1;
        writeImage.pImageInfo = &imageInfo;

        vector<VkWriteDescriptorSet> writeSets = { writeAS, writeImage };
        vkUpdateDescriptorSets(device, static_cast<uint32_t>(writeSets.size()), writeSets.data(), 0, nullptr);
    }

    void RenderAPIVulkan::CreateRTSceneData(uint32_t id)
    {
        auto rtPipeline = rtPipelines[id];

        // 创建DescriptorPool
        vector<VkDescriptorPoolSize> poolSizes = {};

        // 场景中各对象的材质信息
        VkDescriptorPoolSize asPoolSize = {};
        asPoolSize.type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        asPoolSize.descriptorCount = MAX_FRAMES_IN_FLIGHT;
        poolSizes.push_back(asPoolSize);

        // 场景中所有的纹理
        VkDescriptorPoolSize imagePoolSize = {};
        imagePoolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        imagePoolSize.descriptorCount = MAX_FRAMES_IN_FLIGHT * rtSceneTextureNum; // Todo: 这个数量可能需要动态扩展
        poolSizes.push_back(imagePoolSize);

        // 场景中的所有CubeMap
        VkDescriptorPoolSize cubeMapPoolSize = {};
        cubeMapPoolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        cubeMapPoolSize.descriptorCount = MAX_FRAMES_IN_FLIGHT * rtSceneCubeMapNum; // Todo: 这个数量可能需要动态扩展
        poolSizes.push_back(cubeMapPoolSize);

        VkDescriptorPoolCreateInfo poolInfo = {};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.pPoolSizes = poolSizes.data();
        poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
        poolInfo.maxSets = MAX_FRAMES_IN_FLIGHT;
        poolInfo.flags = 0;
        if (vkCreateDescriptorPool(device, &poolInfo, nullptr, &rtPipeline->sceneData.descriptorPool) != VK_SUCCESS)
            throw std::runtime_error("Failed to create descriptor pool for ray tracing!");

        // 创建DescriptorSet
        vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, rtPipeline->pipeline.sceneDescriptorSetLayout);
        rtPipeline->sceneData.descriptorSets = CreateDescriptorSets(rtPipeline->sceneData.descriptorPool, layouts);

        // 创建渲染对象数据索引Buffer
        vector<VkWriteDescriptorSet> writeDescriptorSets;
        rtPipeline->sceneData.dataReferenceBuffers.resize(MAX_FRAMES_IN_FLIGHT);
        VkDeviceSize bufferSize = sizeof(VulkanRTRendererDataReference) * rtSceneRenderObjectNum; // Todo: 这个数量可能需要动态扩展
        for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
        {
            rtPipeline->sceneData.dataReferenceBuffers[i] = CreateBuffer(bufferSize,
                VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VMA_MEMORY_USAGE_AUTO_PREFER_HOST, true);

            VkDescriptorBufferInfo bufferInfo = {};
            bufferInfo.buffer = rtPipeline->sceneData.dataReferenceBuffers[i].buffer;
            bufferInfo.offset = 0;
            bufferInfo.range = bufferSize;
            VkWriteDescriptorSet writeDescriptorSet = {};
            writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
            writeDescriptorSet.dstSet = rtPipeline->sceneData.descriptorSets[i];
            writeDescriptorSet.dstBinding = 0;
            writeDescriptorSet.dstArrayElement = 0;
            writeDescriptorSet.descriptorCount = 1;
            writeDescriptorSet.pBufferInfo = &bufferInfo;
            writeDescriptorSets.push_back(writeDescriptorSet);
        }
        // 把刚刚创建的渲染对象数据索引Buffer绑定到描述符集上
        vkUpdateDescriptorSets(device, static_cast<uint32_t>(writeDescriptorSets.size()), writeDescriptorSets.data(), 0, nullptr);
    }

    void RenderAPIVulkan::UpdateRTSceneData(uint32_t id)
    {
        auto rtPipeline = rtPipelines[id];

        // 遍历场景中的所有纹理，生成对应的VkDescriptorImageInfo
        vector<VkDescriptorImageInfo> imageInfos = {};

        // 填入场景中的纹理
        for (auto textureID : curRTSceneTextureIndexes)
        {
            auto texture = GetTextureByIndex(textureID);

            VkDescriptorImageInfo imageInfo = {};
            imageInfo.sampler = texture->sampler;
            imageInfo.imageView = texture->imageView;
            imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

            imageInfos.push_back(imageInfo);
        }

        // 随便用一个纹理补齐创建VkPipelineLayout时，sceneDescriptorSetLayout中指定的rtSceneTextureNum个纹理数组
        // Todo: 其实最理想的情况是管线设置的纹理数组数量，和实际场景中的数量是一致的，就不用这样搞了
        // 但是场景中的纹理数量是动态的，而管线设置的纹理数组数量是静态的，如果要保持一致，感觉就得重建管线
        // 所以这里先用一个纹理补齐，感觉这样比重建管线好点，但是我也不太清楚最好的方案是什么
        for (size_t i = curRTSceneTextureIndexes.size(); i < rtSceneTextureNum; i++)
        {
            auto texture = GetTextureByIndex(curRTSceneTextureIndexes[0]);

            VkDescriptorImageInfo imageInfo = {};
            imageInfo.sampler = texture->sampler;
            imageInfo.imageView = texture->imageView;
            imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

            imageInfos.push_back(imageInfo);
        }

        // 更新所有纹理绑定的写入信息
        VkWriteDescriptorSet writeImages = {};
        writeImages.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        writeImages.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        writeImages.dstSet = rtPipeline->sceneData.descriptorSets[currentFrame];
        writeImages.dstBinding = 1;
        writeImages.descriptorCount = static_cast<uint32_t>(imageInfos.size());
        writeImages.pImageInfo = imageInfos.data();

        // 所有的CubeMap信息
        vector<VkDescriptorImageInfo> cubeMapInfos = {};

        // 填入场景中的CubeMap
        for (auto cubeMapID : curRTSceneCubeMapIndexes)
        {
			auto cubeMap = GetTextureByIndex(cubeMapID);

			VkDescriptorImageInfo cubeMapInfo = {};
			cubeMapInfo.sampler = cubeMap->sampler;
			cubeMapInfo.imageView = cubeMap->imageView;
			cubeMapInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

			cubeMapInfos.push_back(cubeMapInfo);
		}

        // 随便用一个CubeMap补齐创建VkPipelineLayout时，sceneDescriptorSetLayout中指定的rtSceneCubeMapNum个CubeMap数组
        for (size_t i = curRTSceneCubeMapIndexes.size(); i < rtSceneCubeMapNum; i++)
        {
            auto cubeMap = GetTextureByIndex(curRTSceneCubeMapIndexes[0]);

            VkDescriptorImageInfo cubeMapInfo = {};
            cubeMapInfo.sampler = cubeMap->sampler;
            cubeMapInfo.imageView = cubeMap->imageView;
            cubeMapInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

            cubeMapInfos.push_back(cubeMapInfo);
        }

        // 更新所有CubeMap绑定的写入信息
        VkWriteDescriptorSet writeCubeMaps = {};
        writeCubeMaps.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        writeCubeMaps.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        writeCubeMaps.dstSet = rtPipeline->sceneData.descriptorSets[currentFrame];
        writeCubeMaps.dstBinding = 2;
        writeCubeMaps.descriptorCount = static_cast<uint32_t>(cubeMapInfos.size());
        writeCubeMaps.pImageInfo = cubeMapInfos.data();

        // 重新绑定纹理
        vector<VkWriteDescriptorSet> writeDescriptorSets = { writeImages, writeCubeMaps };
        vkUpdateDescriptorSets(device, static_cast<uint32_t>(writeDescriptorSets.size()), writeDescriptorSets.data(), 0, nullptr);

        // 更新渲染对象的数据引用Buffer
        vector<VulkanRTRendererDataReference> dataReferences = {};
        for (auto& iter : asInstanceData)
        {
            auto meshBuffer = GetVAOByIndex(iter.VAO);
            auto rtMaterial = GetRTMaterialDataByIndex(iter.rtMaterialDataID);

            VulkanRTRendererDataReference dataReference = {};
            dataReference.indexAddress = meshBuffer->indexBufferDeviceAddress;
            dataReference.vertexAddress = meshBuffer->vertexBufferDeviceAddress;
            dataReference.materialAddress = rtMaterial->buffers[currentFrame].deviceAddress;

            dataReferences.push_back(dataReference);
        }

        // 更新当前帧的渲染对象数据引用Buffer
        auto dataReferencePtr = rtPipeline->sceneData.dataReferenceBuffers[currentFrame].mappedAddress;
        memcpy(dataReferencePtr, dataReferences.data(), sizeof(VulkanRTRendererDataReference) * dataReferences.size());
    }


    uint32_t RenderAPIVulkan::GetCurFrameBufferIndex()
    {
        if (curFBOIdx == presentFBOIdx)
            return curPresentImageIdx;
        else
            return currentFrame;
    }

    uint32_t RenderAPIVulkan::GetMipMapLevels(int width, int height)
    {
        // 计算mipmap等级
        // 通常把图片高宽缩小一半就是一级，直到缩不动
        // 先max找出高宽像素里比较大的，然后用log2计算可以被2除几次，再向下取整就是这张图可以缩小多少次了
        // 最后加1是因为原图也要一个等级
        return static_cast<uint32_t>(std::floor(std::log2(std::max(width, height)))) + 1;
    }

    VkTransformMatrixKHR RenderAPIVulkan::GetVkTransformMatrix(const Matrix4& mat)
    {
        float* elements = new float[16];
        mat.ToRowMajorArray(elements);

        VkTransformMatrixKHR transform = {};
        transform.matrix[0][0] = elements[0];
        transform.matrix[0][1] = elements[1];
        transform.matrix[0][2] = elements[2];
        transform.matrix[0][3] = elements[3];
        transform.matrix[1][0] = elements[4];
        transform.matrix[1][1] = elements[5];
        transform.matrix[1][2] = elements[6];
        transform.matrix[1][3] = elements[7];
        transform.matrix[2][0] = elements[8];
        transform.matrix[2][1] = elements[9];
        transform.matrix[2][2] = elements[10];
        transform.matrix[2][3] = elements[11];

        return transform;
    }

    void RenderAPIVulkan::InitImmediateCommand()
    {
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = commandPool;
        allocInfo.commandBufferCount = 1;
        vkAllocateCommandBuffers(device, &allocInfo, &immediateExeCmd);

        VkFenceCreateInfo fenceInfo{};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        // 创建时立刻设置为signaled状态(否则第一次永远等不到)
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
        vkCreateFence(device, &fenceInfo, nullptr, &immediateExeFence);
    }

    void RenderAPIVulkan::ImmediatelyExecute(std::function<void(VkCommandBuffer cmd)>&& function)
    {
        vkResetFences(device, 1, &immediateExeFence);

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        vkBeginCommandBuffer(immediateExeCmd, &beginInfo);

        function(immediateExeCmd);

        vkEndCommandBuffer(immediateExeCmd);

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &immediateExeCmd;
        vkQueueSubmit(graphicsQueue, 1, &submitInfo, immediateExeFence);

        vkWaitForFences(device, 1, &immediateExeFence, VK_TRUE, UINT64_MAX);
    }

    void RenderAPIVulkan::TransitionImageLayout(VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout, VkImageAspectFlags aspectMask, VkPipelineStageFlags srcStage, VkAccessFlags srcAccessMask, VkPipelineStageFlags dstStage, VkAccessFlags dstAccessMask)
    {
        ImmediatelyExecute([=](VkCommandBuffer cmd)
        {
            VkImageMemoryBarrier barrier = {};
            barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
            barrier.oldLayout = oldLayout;
            barrier.newLayout = newLayout;
            // 这两个参数是用于转换队列簇所有权的，如果我们不做这个转换，一定要明确填入VK_QUEUE_FAMILY_IGNORED
            barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            barrier.image = image;
            // 默认处理所有mipmap
            barrier.subresourceRange.baseMipLevel = 0;
            barrier.subresourceRange.levelCount = VK_REMAINING_MIP_LEVELS;
            // 默认处理所有Layer
            barrier.subresourceRange.baseArrayLayer = 0;
            barrier.subresourceRange.layerCount = VK_REMAINING_ARRAY_LAYERS;
            // Image用途(Color, Depth, Stencil)
            barrier.subresourceRange.aspectMask = aspectMask;
            barrier.srcAccessMask = srcAccessMask;
            barrier.dstAccessMask = dstAccessMask;

            vkCmdPipelineBarrier(cmd, srcStage, dstStage,
                // 这个参数填0或者VK_DEPENDENCY_BY_REGION_BIT，后者意味着允许读取到目前为止已写入的资源部分
                0,
                // VkMemoryBarrier数组
                0, nullptr,
                // VkBufferMemoryBarrier数组
                0, nullptr,
                // VkImageMemoryBarrier数组
                1, &barrier
            );
        });
    }

    void RenderAPIVulkan::TransitionImageLayout(VkCommandBuffer cmd, VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout, VkImageAspectFlags aspectMask, VkPipelineStageFlags srcStage, VkAccessFlags srcAccessMask, VkPipelineStageFlags dstStage, VkAccessFlags dstAccessMask)
    {
        VkImageMemoryBarrier barrier = {};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout = oldLayout;
        barrier.newLayout = newLayout;
        // 这两个参数是用于转换队列簇所有权的，如果我们不做这个转换，一定要明确填入VK_QUEUE_FAMILY_IGNORED
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = image;
        // 默认处理所有mipmap
        barrier.subresourceRange.baseMipLevel = 0;
        barrier.subresourceRange.levelCount = VK_REMAINING_MIP_LEVELS;
        // 默认处理所有Layer
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = VK_REMAINING_ARRAY_LAYERS;
        // Image用途(Color, Depth, Stencil)
        barrier.subresourceRange.aspectMask = aspectMask;
        barrier.srcAccessMask = srcAccessMask;
        barrier.dstAccessMask = dstAccessMask;

        vkCmdPipelineBarrier(cmd, srcStage, dstStage,
            // 这个参数填0或者VK_DEPENDENCY_BY_REGION_BIT，后者意味着允许读取到目前为止已写入的资源部分
            0,
            // VkMemoryBarrier数组
            0, nullptr,
            // VkBufferMemoryBarrier数组
            0, nullptr,
            // VkImageMemoryBarrier数组
            1, &barrier
        );
    }

    VkPipelineInputAssemblyStateCreateInfo RenderAPIVulkan::GetAssemblyInfo(VkPrimitiveTopology topology)
    {
        VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo = {};
        inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssemblyInfo.topology = topology;
        inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;
        return inputAssemblyInfo;
    }

    VkPipelineRasterizationStateCreateInfo RenderAPIVulkan::GetRasterizationInfo(VkCullModeFlagBits cullMode)
    {
        VkPipelineRasterizationStateCreateInfo rasterizationInfo = {};
        rasterizationInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        // 如果depthClampEnable设置为VK_TRUE，超过远近裁剪面的片元会进行收敛，而不是丢弃它们
        rasterizationInfo.depthClampEnable = VK_FALSE;
        // 如果rasterizerDiscardEnable设置为VK_TRUE，那么几何图元永远不会传递到光栅化阶段
        // 这是禁止任何数据输出到framebuffer的方法
        rasterizationInfo.rasterizerDiscardEnable = VK_FALSE;
        // 设置片元如何从几何模型中产生，如果不是FILL，需要开启GPU feature
        // VK_POLYGON_MODE_FILL: 多边形区域填充
        // VK_POLYGON_MODE_LINE: 多边形边缘线框绘制
        // VK_POLYGON_MODE_POINT : 多边形顶点作为描点绘制
        rasterizationInfo.polygonMode = VK_POLYGON_MODE_FILL;
        rasterizationInfo.lineWidth = 1.0f;
        rasterizationInfo.cullMode = cullMode;
        rasterizationInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
        // 渲染阴影的偏移配置
        rasterizationInfo.depthBiasEnable = VK_FALSE;
        rasterizationInfo.depthBiasConstantFactor = 0.0f;
        rasterizationInfo.depthBiasClamp = 0.0f;
        rasterizationInfo.depthBiasSlopeFactor = 0.0f;
        return rasterizationInfo;
    }

    VkPipelineMultisampleStateCreateInfo RenderAPIVulkan::GetPipelineMultisampleInfo(VkSampleCountFlagBits rasterizationSamples)
    {
        VkPipelineMultisampleStateCreateInfo multisampleInfo = {};
        multisampleInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampleInfo.sampleShadingEnable = (rasterizationSamples & VK_SAMPLE_COUNT_1_BIT ? VK_FALSE : VK_TRUE);
        multisampleInfo.rasterizationSamples = rasterizationSamples;
        // 这个是调整sampleShading效果的，越接近1效果越平滑，越接近0性能越好
        multisampleInfo.minSampleShading = 1.0f;
        multisampleInfo.pSampleMask = VK_NULL_HANDLE;
        multisampleInfo.alphaToCoverageEnable = VK_FALSE;
        multisampleInfo.alphaToOneEnable = VK_FALSE;
        return multisampleInfo;
    }

    VkDescriptorSetLayout RenderAPIVulkan::CreateDescriptorSetLayout(const ShaderInfo& info)
    {
        vector<VkDescriptorSetLayoutBinding> bindings = {};

        if (!info.vertProperties.baseProperties.empty())
        {
            VkDescriptorSetLayoutBinding uboBinding = {};
            uboBinding.binding = info.vertProperties.baseProperties[0].binding;
            uboBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            uboBinding.descriptorCount = 1;
            uboBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
            bindings.push_back(uboBinding);
        }

        for (auto& texture : info.vertProperties.textureProperties)
        {
            VkDescriptorSetLayoutBinding samplerBinding = {};
            samplerBinding.binding = texture.binding;
            samplerBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            samplerBinding.descriptorCount = 1;
            samplerBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
            bindings.push_back(samplerBinding);
        }

        if (!info.geomProperties.baseProperties.empty())
        {
            VkDescriptorSetLayoutBinding uboBinding = {};
            uboBinding.binding = info.geomProperties.baseProperties[0].binding;
            uboBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            uboBinding.descriptorCount = 1;
            uboBinding.stageFlags = VK_SHADER_STAGE_GEOMETRY_BIT;
            bindings.push_back(uboBinding);
        }

        for (auto& texture : info.geomProperties.textureProperties)
        {
            VkDescriptorSetLayoutBinding samplerBinding = {};
            samplerBinding.binding = texture.binding;
            samplerBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            samplerBinding.descriptorCount = 1;
            samplerBinding.stageFlags = VK_SHADER_STAGE_GEOMETRY_BIT;
            bindings.push_back(samplerBinding);
        }

        if (!info.fragProperties.baseProperties.empty())
        {
            VkDescriptorSetLayoutBinding uboBinding = {};
            uboBinding.binding = info.fragProperties.baseProperties[0].binding;
            uboBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            uboBinding.descriptorCount = 1;
            uboBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
            bindings.push_back(uboBinding);
        }

        for (auto& texture : info.fragProperties.textureProperties)
        {
            VkDescriptorSetLayoutBinding samplerBinding = {};
            samplerBinding.binding = texture.binding;
            samplerBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            samplerBinding.descriptorCount = 1;
            samplerBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
            bindings.push_back(samplerBinding);
        }

        return CreateDescriptorSetLayout(bindings);
    }

    VkDescriptorSetLayout RenderAPIVulkan::CreateDescriptorSetLayout(const vector<VkDescriptorSetLayoutBinding>& bindings)
    {
        VkDescriptorSetLayout descriptorSetLayout = {};
        VkDescriptorSetLayoutCreateInfo layoutInfo = {};
        layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo.pBindings = bindings.data();
        layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
        if (vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS)
            throw std::runtime_error("failed to create descriptor set layout!");

        return descriptorSetLayout;
    }

    VkPipelineLayout RenderAPIVulkan::CreatePipelineLayout(const vector<VkDescriptorSetLayout>& descriptorSetLayouts, const vector<VkPushConstantRange>& pushConstantRanges)
    {
        VkPipelineLayout pipelineLayout = {};
        VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
        pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
        pipelineLayoutInfo.pPushConstantRanges = pushConstantRanges.data();
        pipelineLayoutInfo.pushConstantRangeCount = static_cast<uint32_t>(pushConstantRanges.size());
        if (vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
            throw std::runtime_error("failed to create pipeline layout!");

        return pipelineLayout;
    }
}