#include "RenderAPIVulkan.h"
#include "RenderEngine.h"
#include "GlobalData.h"
#include <stb_image.h>
#include "ShaderParser.h"
#include "Resources.h"
#include "Texture.h"
#include "ZShader.h"
#include "Material.h"
#include "MaterialData.h"

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
        // 返回一个布尔值，表明触发validation layer消息的Vulkan调用是否应被中止
        // 如果返回true，则调用将以VK_ERROR_VALIDATION_FAILED_EXT错误中止
        // 这通常用于测试validation layers本身，所以我们总是返回VK_FALSE
        return VK_FALSE;
    }

    RenderAPIVulkan::RenderAPIVulkan()
    {
        CreateVkInstance();
        CreateDebugMessenger();
        CreateSurface();
        CreatePhysicalDevice();
        CreateLogicalDevice();
        CreateMemoryAllocator();
        CreateCommandPool();
        CreateSwapChain();
        CreateAllRenderPass();
        CreatePresentFrameBuffer();

        presentImageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
        VkSemaphoreCreateInfo semaphoreInfo{};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
            if (vkCreateSemaphore(device, &semaphoreInfo, nullptr, &presentImageAvailableSemaphores[i]) != VK_SUCCESS)
                throw std::runtime_error("failed to create synchronization objects for a frame!");

        InitImmediateCommand();
    }

    void RenderAPIVulkan::BeginFrame()
    {
        VkResult result = vkAcquireNextImageKHR(device, swapChain, UINT64_MAX, presentImageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &curPresentImageIdx);
        // 交换链和Surface已经不兼容了，不能继续用了，一般是窗口大小变化导致的
        if (result == VK_ERROR_OUT_OF_DATE_KHR)
        {
            // 重新创建交换链来适配新的Surface
            // 暂未实现
            Debug::LogError("Need to implement swap chain recreation !");
            return;
        }
        else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
            throw std::runtime_error("failed to acquire swap chain image!");
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
        {
            windowResized = false;
            // 重新创建交换链来适配新的Surface
            // 暂未实现
            Debug::LogError("Need to implement swap chain recreation !");
        }
        else if (result != VK_SUCCESS)
            throw std::runtime_error("failed to present swap chain image!");

        currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
    }

    void RenderAPIVulkan::SetRenderState(RenderStateSetting* state)
    {
        // Vulkan不需要实现这个接口
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
        viewPortInfo.yOffset = yOffset;
    }

    void RenderAPIVulkan::ClearFrameBuffer(const ClearInfo& clearInfo)
    {
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
            if (clearInfo.clearFlags & ZX_CLEAR_FRAME_BUFFER_COLOR_BIT)
            {
                VkClearValue clearValue = {};
                clearValue.color = { clearInfo.color.r, clearInfo.color.g, clearInfo.color.b, clearInfo.color.a };
                VkClearAttachment clearColorAttachment = {};
                clearColorAttachment.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                clearColorAttachment.colorAttachment = 0;
                clearColorAttachment.clearValue = clearValue;
                clearAttachments.push_back(clearColorAttachment);
            }
            if (clearInfo.clearFlags & ZX_CLEAR_FRAME_BUFFER_DEPTH_BIT)
            {
                VkClearValue clearValue = {};
                clearValue.depthStencil = { clearInfo.depth, 0 };
                VkClearAttachment clearDepthAttachment = {};
                clearDepthAttachment.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
                clearDepthAttachment.clearValue = clearValue;
                clearAttachments.push_back(clearDepthAttachment);
            }
            if (clearInfo.clearFlags & ZX_CLEAR_FRAME_BUFFER_STENCIL_BIT)
            {
                VkClearValue clearValue = {};
                clearValue.depthStencil = { 0, clearInfo.stencil };
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
        auto texture = GetTextureByIndex(id);
        DestroyImageView(texture->imageView);
        DestroyImage(texture->image);
        texture->inUse = false;
    }

    ShaderReference* RenderAPIVulkan::LoadAndSetUpShader(const char* path, FrameBufferType type)
    {
        string shaderCode = Resources::LoadTextFile(path);
        auto shaderInfo = ShaderParser::GetShaderInfo(shaderCode);

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
    void RenderAPIVulkan::SetUpMaterial(ShaderReference* shaderReference, MaterialData* materialData)
    {
        auto pipeline = GetPipelineByIndex(shaderReference->ID);

        auto vulkanMaterialData = GetMaterialDataByIndex(materialData->GetID());
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

            for (auto& matTexture : materialData->textures)
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

        materialData->initialized = true;
    }

    void RenderAPIVulkan::DeleteShader(uint32_t id)
    {
        auto pipeline = GetPipelineByIndex(id);

        vkDestroyDescriptorSetLayout(device, pipeline->descriptorSetLayout, VK_NULL_HANDLE);
        vkDestroyPipeline(device, pipeline->pipeline, VK_NULL_HANDLE);
        vkDestroyPipelineLayout(device, pipeline->pipelineLayout, VK_NULL_HANDLE);

        pipeline->inUse = false;
    }

    void RenderAPIVulkan::DeleteMaterialData(uint32_t id)
    {
        auto vulkanMaterialData = GetMaterialDataByIndex(id);

        vkDestroyDescriptorPool(device, vulkanMaterialData->descriptorPool, VK_NULL_HANDLE);

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

    FrameBufferObject* RenderAPIVulkan::CreateFrameBufferObject(FrameBufferType type, unsigned int width, unsigned int height)
    {
        width = width == 0 ? GlobalData::srcWidth : width;
        height = height == 0 ? GlobalData::srcHeight : height;
        FrameBufferObject* FBO = new FrameBufferObject(type);

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
        else
        {
            Debug::LogError("Invalide frame buffer type.");
        }

        return FBO;
    }

    uint32_t RenderAPIVulkan::AllocateDrawCommand()
    {
        uint32_t idx = GetNextDrawCommandIndex();
        auto drawCmd = GetDrawCommandByIndex(idx);
        drawCmd->inUse = true;
        return idx;
    }

    void RenderAPIVulkan::Draw(uint32_t VAO)
    {
        drawIndexes.push_back({ .VAO = VAO, .pipelineID = curPipeLineIdx, .materialDataID = curMaterialDataIdx });
    }

    void RenderAPIVulkan::GenerateDrawCommand(uint32_t id)
    {
        auto curDrawCommand = GetDrawCommandByIndex(id);
        auto commandBuffer = curDrawCommand->commandBuffers[currentFrame];

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
        renderPassInfo.pClearValues = VK_NULL_HANDLE;
        renderPassInfo.clearValueCount = 0;
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

            auto commandBuffer = curDrawCommand->commandBuffers[currentFrame];

            VkBuffer vertexBuffers[] = { vulkanVAO->vertexBuffer };
            VkDeviceSize offsets[] = { 0 };
            vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

            vkCmdBindIndexBuffer(commandBuffer, vulkanVAO->indexBuffer, 0, VK_INDEX_TYPE_UINT32);

            vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->pipeline);

            vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->pipelineLayout, 0, 1, &materialData->descriptorSets[currentFrame], 0, VK_NULL_HANDLE);

            vkCmdDrawIndexed(commandBuffer, vulkanVAO->size, 1, 0, 0, 0);
        }

        vkCmdEndRenderPass(commandBuffer);

        if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
            throw std::runtime_error("failed to record command buffer!");

        VkSubmitInfo submitInfo = {};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.pCommandBuffers = &commandBuffer;
        submitInfo.commandBufferCount = 1;
        submitInfo.pWaitSemaphores = VK_NULL_HANDLE;
        submitInfo.pWaitDstStageMask = VK_NULL_HANDLE;
        submitInfo.waitSemaphoreCount = 0;
        submitInfo.pSignalSemaphores = VK_NULL_HANDLE;
        submitInfo.signalSemaphoreCount = 0;

        if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS)
            throw std::runtime_error("failed to submit draw command buffer!");

        // 先临时这样处理一下同步问题
        vkQueueWaitIdle(graphicsQueue);

        drawIndexes.clear();
    }

    void RenderAPIVulkan::DeleteMesh(unsigned int VAO)
    {
        auto meshBuffer = GetVAOByIndex(VAO);
        meshBuffer->inUse = false;

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
    }

    void RenderAPIVulkan::SetUpStaticMesh(unsigned int& VAO, const vector<Vertex>& vertices, const vector<uint32_t>& indices)
    {
        VAO = GetNextVAOIndex();
        auto meshBuffer = GetVAOByIndex(VAO);
        meshBuffer->size = static_cast<uint32_t>(indices.size());

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
        VkBufferCreateInfo vertexBufferInfo = {};
        vertexBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        vertexBufferInfo.size = vertexBufferSize;
        vertexBufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
        vertexBufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE; // 只有一个队列簇使用

        VmaAllocationCreateInfo vertexBufferAllocInfo = {};
        vertexBufferAllocInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;

        vmaCreateBuffer(vmaAllocator, &vertexBufferInfo, &vertexBufferAllocInfo, &meshBuffer->vertexBuffer, &meshBuffer->vertexBufferAlloc, nullptr);

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
        VkBufferCreateInfo indexBufferInfo = {};
        indexBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        indexBufferInfo.size = indexBufferSize;
        indexBufferInfo.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
        indexBufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE; // 只有一个队列簇使用

        VmaAllocationCreateInfo indexBufferAllocInfo = {};
        indexBufferAllocInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;

        vmaCreateBuffer(vmaAllocator, &indexBufferInfo, &indexBufferAllocInfo, &meshBuffer->indexBuffer, &meshBuffer->indexBufferAlloc, nullptr);

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

        meshBuffer->inUse = true;
    }

    void RenderAPIVulkan::SetUpDynamicMesh(unsigned int& VAO, unsigned int vertexSize, unsigned int indexSize)
    {
        VAO = GetNextVAOIndex();
        auto meshBuffer = GetVAOByIndex(VAO);
        meshBuffer->size = indexSize;

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
            { {  0.5f,  0.5f, 0.0f }, { 0.0f, 1.0f, 0.0f }, { 1.0f, 1.0f } },
            { {  0.5f, -0.5f, 0.0f }, { 0.0f, 1.0f, 0.0f }, { 1.0f, 0.0f } },
            { { -0.5f,  0.5f, 0.0f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 1.0f } },
            { { -0.5f, -0.5f, 0.0f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f } },
        };

        vector<uint32_t> indices =
        {
            2, 1, 3,
            2, 0, 1,
        };

        SetUpStaticMesh(VAO, vertices, indices);
    }

    void RenderAPIVulkan::UseShader(unsigned int ID)
    {
        curPipeLineIdx = ID;
    }
    void RenderAPIVulkan::SetShaderScalar(Material* material, const string& name, bool value, bool allBuffer)
    {
        if (allBuffer)
        {
            auto valueAddresses = GetShaderPropertyAddressAllBuffer(material->shader->reference, material->data->GetID(), name);
            for (auto valueAddress : valueAddresses)
                memcpy(valueAddress, &value, sizeof(value));
        }
        else
        {
            void* valueAddress = GetShaderPropertyAddress(material->shader->reference, material->data->GetID(), name);
            memcpy(valueAddress, &value, sizeof(value));
        }
    }
    void RenderAPIVulkan::SetShaderScalar(Material* material, const string& name, int value, bool allBuffer)
    {
        if (allBuffer)
        {
            auto valueAddresses = GetShaderPropertyAddressAllBuffer(material->shader->reference, material->data->GetID(), name);
            for (auto valueAddress : valueAddresses)
                memcpy(valueAddress, &value, sizeof(value));
        }
        else
        {
            void* valueAddress = GetShaderPropertyAddress(material->shader->reference, material->data->GetID(), name);
            memcpy(valueAddress, &value, sizeof(value));
        }
    }
    void RenderAPIVulkan::SetShaderScalar(Material* material, const string& name, float value, bool allBuffer)
    {
        if (allBuffer)
        {
            auto valueAddresses = GetShaderPropertyAddressAllBuffer(material->shader->reference, material->data->GetID(), name);
            for (auto valueAddress : valueAddresses)
                memcpy(valueAddress, &value, sizeof(value));
        }
        else
        {
            void* valueAddress = GetShaderPropertyAddress(material->shader->reference, material->data->GetID(), name);
            memcpy(valueAddress, &value, sizeof(value));
        }
    }
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
            auto valueAddresses = GetShaderPropertyAddressAllBuffer(material->shader->reference, material->data->GetID(), name, idx);
            for (auto valueAddress : valueAddresses)
                memcpy(valueAddress, array, sizeof(float) * 2);
        }
        else
        {
            void* valueAddress = GetShaderPropertyAddress(material->shader->reference, material->data->GetID(), name, idx);
            memcpy(valueAddress, array, sizeof(float) * 2);
        }
        delete[] array;
    }
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
            auto valueAddresses = GetShaderPropertyAddressAllBuffer(material->shader->reference, material->data->GetID(), name, idx);
            for (auto valueAddress : valueAddresses)
                memcpy(valueAddress, array, sizeof(float) * 3);
        }
        else
        {
            void* valueAddress = GetShaderPropertyAddress(material->shader->reference, material->data->GetID(), name, idx);
            memcpy(valueAddress, array, sizeof(float) * 3);
        }
        delete[] array;
    }
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
            auto valueAddresses = GetShaderPropertyAddressAllBuffer(material->shader->reference, material->data->GetID(), name, idx);
            for (auto valueAddress : valueAddresses)
                memcpy(valueAddress, array, sizeof(float) * 4);
        }
        else
        {
            void* valueAddress = GetShaderPropertyAddress(material->shader->reference, material->data->GetID(), name, idx);
            memcpy(valueAddress, array, sizeof(float) * 4);
        }
        delete[] array;
    }
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
            auto valueAddresses = GetShaderPropertyAddressAllBuffer(material->shader->reference, material->data->GetID(), name, idx);
            for (auto valueAddress : valueAddresses)
                memcpy(valueAddress, array, sizeof(float) * 9);
        }
        else
        {
            void* valueAddress = GetShaderPropertyAddress(material->shader->reference, material->data->GetID(), name, idx);
            memcpy(valueAddress, array, sizeof(float) * 9);
        }
        delete[] array;
    }
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
            auto valueAddresses = GetShaderPropertyAddressAllBuffer(material->shader->reference, material->data->GetID(), name, idx);
            for (auto valueAddress : valueAddresses)
                memcpy(valueAddress, array, sizeof(float) * 16);
        }
        else
        {
            void* valueAddress = GetShaderPropertyAddress(material->shader->reference, material->data->GetID(), name, idx);
            memcpy(valueAddress, array, sizeof(float) * 16);
        }
        delete[] array;
    }

    // Vulkan不需要第4个参数
    void RenderAPIVulkan::SetShaderTexture(Material* material, const string& name, uint32_t ID, uint32_t idx, bool isBuffer)
    {
        auto vulkanMaterialData = GetMaterialDataByIndex(material->data->GetID());

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
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

    void RenderAPIVulkan::SetShaderCubeMap(Material* material, const string& name, uint32_t ID, uint32_t idx, bool isBuffer)
    {
        SetShaderTexture(material, name, ID, idx, isBuffer);
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

    uint32_t RenderAPIVulkan::GetNextDrawCommandIndex()
    {
        uint32_t length = (uint32_t)VulkanDrawCommandArray.size();

        for (uint32_t i = 0; i < length; i++)
        {
            if (!VulkanDrawCommandArray[i]->inUse)
                return i;
        }

        auto newDrawCommand = new VulkanDrawCommand();
        newDrawCommand->commandBuffers.resize(MAX_FRAMES_IN_FLIGHT);
        AllocateCommandBuffers(newDrawCommand->commandBuffers);

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


    // ------------------------------------------建立各种Vulkan对象--------------------------------------------


    void RenderAPIVulkan::CreateVkInstance()
    {
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
        VkPhysicalDeviceFeatures deviceFeatures = {};
        // 启用对各向异性采样的支持
        deviceFeatures.samplerAnisotropy = VK_TRUE;
        deviceFeatures.geometryShader = VK_TRUE;
        deviceFeatures.sampleRateShading = VK_TRUE;

        // 创建逻辑设备的信息
        VkDeviceCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        // 使用前面的两个结构体填充
        createInfo.pQueueCreateInfos = queueCreateInfos.data();
        createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
        createInfo.pEnabledFeatures = &deviceFeatures;

        // 和VkInstance一样，VkDevice可以开启扩展和验证层
        // 添加扩展信息
        createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
        createInfo.ppEnabledExtensionNames = deviceExtensions.data();
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
        if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS) {
            throw std::runtime_error("failed to create logical device!");
        }

        // 逻辑设备创建的时候，队列也一起创建了，获取队列并保存下来方便之后调用
        // 参数是逻辑设备，队列簇，队列索引和存储获取队列变量句柄的指针
        // 因为我们只是从这个队列簇创建一个队列，所以需要使用索引0
        vkGetDeviceQueue(device, queueFamilyIndices.graphics, 0, &graphicsQueue);
        vkGetDeviceQueue(device, queueFamilyIndices.present, 0, &presentQueue);
    }

    void RenderAPIVulkan::CreateMemoryAllocator()
    {
        VmaAllocatorCreateInfo vmaInfo = {};
        vmaInfo.vulkanApiVersion = VK_HEADER_VERSION_COMPLETE;
        vmaInfo.instance = vkInstance;
        vmaInfo.physicalDevice = physicalDevice;
        vmaInfo.device = device;
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
        if (glfwCreateWindowSurface(vkInstance, RenderEngine::GetInstance()->window, nullptr, &surface) != VK_SUCCESS)
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
        auto vulkanFBO = GetFBOByIndex(presentFBOIdx);
        vulkanFBO->bufferType = FrameBufferType::Present;
        vulkanFBO->renderPassType = RenderPassType::Present;

        vulkanFBO->frameBuffers.clear();
        vulkanFBO->frameBuffers.resize(swapChainImages.size());

        swapChainImageViews.resize(swapChainImages.size());
        for (size_t i = 0; i < swapChainImages.size(); i++)
        {
            swapChainImageViews[i] = CreateImageView(swapChainImages[i], swapChainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_VIEW_TYPE_2D);

            VulkanImage colorImage = CreateImage(swapChainExtent.width, swapChainExtent.height, 1, 1, msaaSamplesCount, swapChainImageFormat, VK_IMAGE_TILING_OPTIMAL,
                VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE);
            VkImageView colorImageView = CreateImageView(colorImage.image, swapChainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_VIEW_TYPE_2D);

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
        VkPhysicalDeviceProperties physicalDeviceProperties;
        vkGetPhysicalDeviceProperties(physicalDevice, &physicalDeviceProperties);

        // 取同时支持Color和Depth的最大数量
        VkSampleCountFlags counts = physicalDeviceProperties.limits.framebufferColorSampleCounts & physicalDeviceProperties.limits.framebufferDepthSampleCounts;
        if (counts & VK_SAMPLE_COUNT_64_BIT) { msaaSamplesCount = VK_SAMPLE_COUNT_64_BIT; }
        else if (counts & VK_SAMPLE_COUNT_32_BIT) { msaaSamplesCount = VK_SAMPLE_COUNT_32_BIT; }
        else if (counts & VK_SAMPLE_COUNT_16_BIT) { msaaSamplesCount = VK_SAMPLE_COUNT_16_BIT; }
        else if (counts & VK_SAMPLE_COUNT_8_BIT) { msaaSamplesCount = VK_SAMPLE_COUNT_8_BIT; }
        else if (counts & VK_SAMPLE_COUNT_4_BIT) { msaaSamplesCount = VK_SAMPLE_COUNT_4_BIT; }
        else if (counts & VK_SAMPLE_COUNT_2_BIT) { msaaSamplesCount = VK_SAMPLE_COUNT_2_BIT; }
        else { msaaSamplesCount = VK_SAMPLE_COUNT_1_BIT; }

        maxSamplerAnisotropy = physicalDeviceProperties.limits.maxSamplerAnisotropy;
        minUniformBufferOffsetAlignment = physicalDeviceProperties.limits.minUniformBufferOffsetAlignment;
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
            glfwGetFramebufferSize(RenderEngine::GetInstance()->window, &width, &height);

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


    VulkanBuffer RenderAPIVulkan::CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VmaMemoryUsage memoryUsage, bool map)
    {
        VkBufferCreateInfo bufferInfo = {};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = size;
        bufferInfo.usage = usage;

        VmaAllocationCreateInfo allocationInfo = {};
        allocationInfo.usage = memoryUsage;
        if (map)
            allocationInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;

        VulkanBuffer newBuffer;
        vmaCreateBuffer(vmaAllocator, &bufferInfo, &allocationInfo, &newBuffer.buffer, &newBuffer.allocation, nullptr);
        return newBuffer;
    }

    void RenderAPIVulkan::DestroyBuffer(VulkanBuffer buffer)
    {
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

    VkFence RenderAPIVulkan::CreateFence()
    {
        VkFence fence;
        VkFenceCreateInfo fenceInfo = {};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        // 创建时立刻设置为signaled状态(否则第一次的vkWaitForFences永远等不到结果)
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
        if (vkCreateFence(device, &fenceInfo, nullptr, &fence) != VK_SUCCESS)
            throw std::runtime_error("failed to create fence objects!");

        return fence;
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
            colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
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
            depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
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
            depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
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
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(Vertex, Normal);
        attributeDescriptions[2].binding = 0;
        attributeDescriptions[2].location = 2;
        attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[2].offset = offsetof(Vertex, TexCoords);
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
        pipelineLayout = CreatePipelineLayout(descriptorSetLayout);

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

        VkDescriptorSetLayout descriptorSetLayout = {};
        VkDescriptorSetLayoutCreateInfo layoutInfo = {};
        layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo.pBindings = bindings.data();
        layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
        if (vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS)
            throw std::runtime_error("failed to create descriptor set layout!");

        return descriptorSetLayout;
    }

    VkPipelineLayout RenderAPIVulkan::CreatePipelineLayout(const VkDescriptorSetLayout& descriptorSetLayout)
    {
        VkPipelineLayout pipelineLayout = {};
        VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout;
        pipelineLayoutInfo.setLayoutCount = 1;
        pipelineLayoutInfo.pPushConstantRanges = nullptr;
        pipelineLayoutInfo.pushConstantRangeCount = 0;
        if (vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
            throw std::runtime_error("failed to create pipeline layout!");

        return pipelineLayout;
    }
}