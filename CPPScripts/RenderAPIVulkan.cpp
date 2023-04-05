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

// VMA�Ĺٷ��ĵ���˵��Ҫ��һ��CPP�ļ��ﶨ������궨�壬������ܻ����쳣
// ��:https://gpuopen-librariesandsdks.github.io/VulkanMemoryAllocator/html/quick_start.html#quick_start_project_setup
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

    // �Զ����Debug�ص�������VKAPI_ATTR��VKAPI_CALLȷ������ȷ�ĺ���ǩ�����Ӷ���Vulkan����
    static VKAPI_ATTR VkBool32 VKAPI_CALL VkDebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
    {
        std::cerr << "My debug call back: " << pCallbackData->pMessage << std::endl;
        // ����һ������ֵ����������validation layer��Ϣ��Vulkan�����Ƿ�Ӧ����ֹ
        // �������true������ý���VK_ERROR_VALIDATION_FAILED_EXT������ֹ
        // ��ͨ�����ڲ���validation layers���������������Ƿ���VK_FALSE
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
        // ��������Surface�Ѿ��������ˣ����ܼ������ˣ�һ���Ǵ��ڴ�С�仯���µ�
        if (result == VK_ERROR_OUT_OF_DATE_KHR)
        {
            // ���´����������������µ�Surface
            // ��δʵ��
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
        // VK_ERROR_OUT_OF_DATE_KHR��ʾ��������Surface�Ѿ��������ˣ����ܼ������ˣ��������´���������
        // VK_SUBOPTIMAL_KHR��ʾ���������ǿ��Լ����ã����Ǻ�Surface��ĳЩ����ƥ��ò��Ǻܺã������´���Ҳ��
        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || windowResized) 
        {
            windowResized = false;
            // ���´����������������µ�Surface
            // ��δʵ��
            Debug::LogError("Need to implement swap chain recreation !");
        }
        else if (result != VK_SUCCESS)
            throw std::runtime_error("failed to present swap chain image!");

        currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
    }

    void RenderAPIVulkan::SetRenderState(RenderStateSetting* state)
    {
        // Vulkan����Ҫʵ������ӿ�
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

        // �����ݿ�����stagingBuffer
        void* data;
        void* pixelsPtr = pixels; // Ϊmemcpyת��һ��ָ������
        vmaMapMemory(vmaAllocator, stagingBuffer.allocation, &data);
        memcpy(data, pixelsPtr, static_cast<size_t>(imageSize));
        vmaUnmapMemory(vmaAllocator, stagingBuffer.allocation);

        stbi_image_free(pixels);

        uint32_t mipLevels = GetMipMapLevels(width, height);

        VulkanImage image = CreateImage(width, height, mipLevels, 1, VK_SAMPLE_COUNT_1_BIT,
            defaultImageFormat, VK_IMAGE_TILING_OPTIMAL,
            // ��������Ҫ��һ��stagingBuffer�������ݣ�����Ҫдһ��VK_IMAGE_USAGE_TRANSFER_DST_BIT
            // ����Ϊ����Ҫ����mipmap����Ҫ�����ԭimage�����ݣ��������ټ�һ��VK_IMAGE_USAGE_TRANSFER_SRC_BIT
            // Ȼ����дһ��VK_IMAGE_USAGE_SAMPLED_BIT��ʾ������shader�����������
            VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
            VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE);

        TransitionImageLayout(image.image, 
            // �ӳ�ʼ��Layoutת��������stagingBuffer���ݵ�Layout
            VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 
            // ��Ӳ�̼��ص�ͼ��Ĭ�϶���Color
            VK_IMAGE_ASPECT_COLOR_BIT,
            // ת������ֱ�ӿ�ʼ��û������
            VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, 0,
            // ͼ��Transfer��д�������Ҫ�����ת��֮�����
            VK_PIPELINE_STAGE_TRANSFER_BIT, VK_ACCESS_TRANSFER_WRITE_BIT);

        // �����ݴ�stagingBuffer���Ƶ�image
        ImmediatelyExecute([=](VkCommandBuffer cmd)
        {
            VkBufferImageCopy region{};
            // ��buffer��ȡ���ݵ���ʼƫ����
            region.bufferOffset = 0;
            // ������������ȷ�������ڴ���Ĳ��ַ�ʽ���������ֻ�Ǽ򵥵Ľ����������ݣ�����0
            region.bufferRowLength = 0;
            region.bufferImageHeight = 0;
            // ����4��������������������Ҫ�����ݿ�����image����һ����
            region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            region.imageSubresource.mipLevel = 0;
            region.imageSubresource.baseArrayLayer = 0;
            region.imageSubresource.layerCount = 1;
            // ���Ҳ������������Ҫ��ͼ�񿽱�����һ����
            // ���������ͼƬ��offset��ȫ��0��extent��ֱ����ͼ��߿�
            region.imageOffset = { 0, 0, 0 };
            region.imageExtent = { (uint32_t)width, (uint32_t)height, 1 };

            vkCmdCopyBufferToImage(
                cmd,
                stagingBuffer.buffer,
                image.image,
                // image��ǰ��layout
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

        // ͬLoadTexture
        TransitionImageLayout(image.image,
            VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            VK_IMAGE_ASPECT_COLOR_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, 0,
            VK_PIPELINE_STAGE_TRANSFER_BIT, VK_ACCESS_TRANSFER_WRITE_BIT);

        vector<VulkanBuffer> stagingBuffers;
        for (uint32_t i = 0; i < 6; i++)
            stagingBuffers.push_back(CreateBuffer(singleImageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_AUTO_PREFER_HOST, true));
        
        // �����ݴ�stagingBuffer���Ƶ�image
        ImmediatelyExecute([=](VkCommandBuffer cmd)
        {
            for (uint32_t i = 0; i < 6; i++)
            {
                // �����ݿ�����stagingBuffer
                void* data;
                void* pixelsPtr = textureData[i]; // Ϊmemcpyת��һ��ָ������
                vmaMapMemory(vmaAllocator, stagingBuffers[i].allocation, &data);
                memcpy(data, pixelsPtr, static_cast<size_t>(singleImageSize));
                vmaUnmapMemory(vmaAllocator, stagingBuffers[i].allocation);

                stbi_image_free(textureData[i]);

                VkBufferImageCopy bufferCopyRegion = {};
                // ��buffer��ȡ���ݵ���ʼƫ����
                bufferCopyRegion.bufferOffset = 0;
                // ������������ȷ�������ڴ���Ĳ��ַ�ʽ���������ֻ�Ǽ򵥵Ľ����������ݣ�����0
                bufferCopyRegion.bufferRowLength = 0;
                bufferCopyRegion.bufferImageHeight = 0;
                // ����4��������������������Ҫ�����ݿ�����image����һ����
                bufferCopyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                bufferCopyRegion.imageSubresource.mipLevel = 0;
                bufferCopyRegion.imageSubresource.baseArrayLayer = i;
                bufferCopyRegion.imageSubresource.layerCount = 1;
                bufferCopyRegion.imageOffset = { 0, 0, 0 };
                bufferCopyRegion.imageExtent = { static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight), 1 };

                vkCmdCopyBufferToImage(cmd,
                    stagingBuffers[i].buffer,
                    image.image,
                    // image��ǰ��layout
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
        // һ���ı�����8bit
        VkDeviceSize imageSize = VkDeviceSize(width * height);
        if (imageSize == 0)
            throw std::runtime_error("Can't create texture with size 0 !");
        VulkanBuffer stagingBuffer = CreateBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_AUTO_PREFER_HOST, true);

        // �����ݿ�����stagingBuffer
        void* ptr;
        vmaMapMemory(vmaAllocator, stagingBuffer.allocation, &ptr);
        memcpy(ptr, data, static_cast<size_t>(imageSize));
        vmaUnmapMemory(vmaAllocator, stagingBuffer.allocation);

        VulkanImage image = CreateImage(width, height, 1, 1, VK_SAMPLE_COUNT_1_BIT,
            VK_FORMAT_R8_SRGB, VK_IMAGE_TILING_OPTIMAL,
            VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
            VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE);

        // ͬLoadTexture
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

    // ���������ɵ�������ͨ��shaderReference�����Ϣ������MaterialData��Ҳ������vkUpdateDescriptorSets��Image��VkPipeline������
    void RenderAPIVulkan::SetUpMaterial(ShaderReference* shaderReference, MaterialData* materialData)
    {
        auto pipeline = GetPipelineByIndex(shaderReference->ID);

        auto vulkanMaterialData = GetMaterialDataByIndex(materialData->GetID());
        vulkanMaterialData->descriptorPool = CreateDescriptorPool(shaderReference->shaderInfo);

        // ����Uniform Buffer
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

        // ��Uniform Buffer�󶨵�DescriptorSet��
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

        // ������󶨵�DescriptorSet��
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
                // ����ܶ�Attachment���涼���ܻ���������Shader���������Զ�����VK_IMAGE_USAGE_SAMPLED_BIT
                // ���ȷ�����ᱻ���������ģ����Բ���VK_IMAGE_USAGE_SAMPLED_BIT���ĳ�VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT�������
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
                // ָ�����Լ��ݵ�render pass(���frame buffer��ָ����render pass��attachment��������������Ҫһ��)
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
        // ���render area������shader��Ҫ���غʹ洢��λ��
        renderPassInfo.renderArea.offset = { viewPortInfo.xOffset, viewPortInfo.yOffset };
        // һ����˵��С(extend)�Ǻ�framebuffer��attachmentһ�µģ����С�˻��˷ѣ����˳���ȥ�Ĳ�����һЩδ������ֵ
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

        // ����ʱ��������һ��ͬ������
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

        // ����StagingBuffer
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

        // �������ݵ�StagingBuffer
        void* vertexData;
        vmaMapMemory(vmaAllocator, vertexStagingBufferAlloc, &vertexData);
        memcpy(vertexData, vertices.data(), vertices.size() * sizeof(Vertex));
        vmaUnmapMemory(vmaAllocator, vertexStagingBufferAlloc);

        // ����VertexBuffer
        VkBufferCreateInfo vertexBufferInfo = {};
        vertexBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        vertexBufferInfo.size = vertexBufferSize;
        vertexBufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
        vertexBufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE; // ֻ��һ�����д�ʹ��

        VmaAllocationCreateInfo vertexBufferAllocInfo = {};
        vertexBufferAllocInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;

        vmaCreateBuffer(vmaAllocator, &vertexBufferInfo, &vertexBufferAllocInfo, &meshBuffer->vertexBuffer, &meshBuffer->vertexBufferAlloc, nullptr);

        // ��StagingBuffer������VertexBuffer
        ImmediatelyExecute([=](VkCommandBuffer cmd)
        {
            VkBufferCopy copy = {};
            copy.dstOffset = 0;
            copy.srcOffset = 0;
            copy.size = vertexBufferSize;
            vkCmdCopyBuffer(cmd, vertexStagingBuffer, meshBuffer->vertexBuffer, 1, &copy);
        });

        // ����StagingBuffer
        vmaDestroyBuffer(vmaAllocator, vertexStagingBuffer, vertexStagingBufferAlloc);

        // ----------------------------------------------- Index Buffer -----------------------------------------------
        VkDeviceSize indexBufferSize = sizeof(unsigned int) * indices.size();

        // ����StagingBuffer
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

        // �������ݵ�StagingBuffer
        void* indexData;
        vmaMapMemory(vmaAllocator, indexStagingBufferAlloc, &indexData);
        memcpy(indexData, indices.data(), indices.size() * sizeof(unsigned int));
        vmaUnmapMemory(vmaAllocator, indexStagingBufferAlloc);

        // ����IndexBuffer
        VkBufferCreateInfo indexBufferInfo = {};
        indexBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        indexBufferInfo.size = indexBufferSize;
        indexBufferInfo.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
        indexBufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE; // ֻ��һ�����д�ʹ��

        VmaAllocationCreateInfo indexBufferAllocInfo = {};
        indexBufferAllocInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;

        vmaCreateBuffer(vmaAllocator, &indexBufferInfo, &indexBufferAllocInfo, &meshBuffer->indexBuffer, &meshBuffer->indexBufferAlloc, nullptr);

        // ��StagingBuffer������IndexBuffer
        ImmediatelyExecute([=](VkCommandBuffer cmd)
        {
            VkBufferCopy copy = {};
            copy.dstOffset = 0;
            copy.srcOffset = 0;
            copy.size = indexBufferSize;
            vkCmdCopyBuffer(cmd, indexStagingBuffer, meshBuffer->indexBuffer, 1, &copy);
        });

        // ����StagingBuffer
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

    // Vulkan����Ҫ��4������
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

            // û�ҵ��Ļ�����
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


    // ------------------------------------------��������Vulkan����--------------------------------------------


    void RenderAPIVulkan::CreateVkInstance()
    {
        validationLayersEnabled = CheckValidationLayerSupport();

        // ������ݿ��Բ������������ݿ����ÿ���������Ӳ�����̣�����Nvidiaʲô��ʶ��һ�£���һЩ���������Ϸ�ߺ��������⴦��ʲô��
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

        // ��ȡ�벻ͬƽ̨�Ĵ���ϵͳ���н�������չ��Ϣ
        auto extensions = GetRequiredExtensions();
        createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
        createInfo.ppEnabledExtensionNames = extensions.data();

        // ���������֤������һ��
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

        // ���������֤�㣬����һ�½���Debug��Ϣ�Ļص�
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
        // ��ȡ��ǰ�豸֧��Vulkan���Կ�����
        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(vkInstance, &deviceCount, nullptr);
        if (deviceCount == 0)
            throw std::runtime_error("failed to find GPUs with Vulkan support!");

        // ��ȡ����֧��Vulkan���Կ�����ŵ�devices��
        vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(vkInstance, &deviceCount, devices.data());

        // ���������Կ����ҵ���һ���������������
        // ��ʵ����д�����߳�ͬʱ������Щ�Կ������������������һ��
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
        // ��ȡ��ǰ�����豸�Ķ��д�����
        queueFamilyIndices = GetQueueFamilyIndices(physicalDevice);

        // �߼��豸��Ҫ��ЩQueue
        vector<VkDeviceQueueCreateInfo> queueCreateInfos;
        float queuePriority = 1.0f;
        set<uint32_t> uniqueQueueFamilies = { queueFamilyIndices.graphics, queueFamilyIndices.present };
        // �ж�����дأ���������
        for (uint32_t queueFamily : uniqueQueueFamilies)
        {
            VkDeviceQueueCreateInfo queueCreateInfo = {};
            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            // ��ǰ���дض�Ӧ������
            queueCreateInfo.queueFamilyIndex = queueFamily;
            // ���д�����Ҫ����ʹ�õĶ�������
            queueCreateInfo.queueCount = 1;
            // Vulkan����ʹ��0.0��1.0֮��ĸ���������������ȼ���Ӱ���������ִ�еĵ��ã���ʹֻ��һ������Ҳ�Ǳ����
            queueCreateInfo.pQueuePriorities = &queuePriority;
            queueCreateInfos.push_back(queueCreateInfo);
        }

        // ��ȷ�豸Ҫʹ�õĹ�������
        VkPhysicalDeviceFeatures deviceFeatures = {};
        // ���öԸ������Բ�����֧��
        deviceFeatures.samplerAnisotropy = VK_TRUE;
        deviceFeatures.geometryShader = VK_TRUE;
        deviceFeatures.sampleRateShading = VK_TRUE;

        // �����߼��豸����Ϣ
        VkDeviceCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        // ʹ��ǰ��������ṹ�����
        createInfo.pQueueCreateInfos = queueCreateInfos.data();
        createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
        createInfo.pEnabledFeatures = &deviceFeatures;

        // ��VkInstanceһ����VkDevice���Կ�����չ����֤��
        // �����չ��Ϣ
        createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
        createInfo.ppEnabledExtensionNames = deviceExtensions.data();
        // �����������֤�㣬����֤����Ϣ��ӽ�ȥ
        if (validationLayersEnabled)
        {
            createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
            createInfo.ppEnabledLayerNames = validationLayers.data();
        }
        else
        {
            createInfo.enabledLayerCount = 0;
        }

        // ����vkCreateDevice����������ʵ�����߼��豸
        // �߼��豸����VkInstanceֱ�ӽ��������Բ�����ֻ�������豸
        if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS) {
            throw std::runtime_error("failed to create logical device!");
        }

        // �߼��豸������ʱ�򣬶���Ҳһ�𴴽��ˣ���ȡ���в�������������֮�����
        // �������߼��豸�����дأ����������ʹ洢��ȡ���б��������ָ��
        // ��Ϊ����ֻ�Ǵ�������дش���һ�����У�������Ҫʹ������0
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
        // ���flags��Ҫ�ɲ�Ҫ�����Ҫ���flags�Ļ���|����
        // VK_COMMAND_POOL_CREATE_TRANSIENT_BIT: ��ʾ��������ǳ�Ƶ�������¼�¼������(���ܻ�ı��ڴ������Ϊ)
        // VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT: ������������������¼�¼��û�������־�����е��������������һ������
        poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        // command buffer��ͨ����һ���豸�������ύ������ִ�еģ�ÿ�������ֻ�ܷ����ڵ�һ���Ͷ������ύ���������
        // ���ǽ���¼���ڻ�ͼ�����������graphicsFamily
        poolInfo.queueFamilyIndex = queueFamilyIndices.graphics;

        if (vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool) != VK_SUCCESS)
            throw std::runtime_error("failed to create command pool!");
    }

    void RenderAPIVulkan::CreateSurface() {
        // surface�ľ��崴��������Ҫ����ƽ̨�ģ�����ֱ����GLFW��װ�õĽӿ�������
        if (glfwCreateWindowSurface(vkInstance, RenderEngine::GetInstance()->window, nullptr, &surface) != VK_SUCCESS)
            throw std::runtime_error("failed to create window surface!");
    }

    void RenderAPIVulkan::CreateSwapChain()
    {
        // ��ѯӲ��֧�ֵĽ���������
        SwapChainSupportDetails swapChainSupport = GetSwapChainSupportDetails(physicalDevice);

        // ѡ��һ��ͼ���ʽ
        VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(swapChainSupport.formats);
        // ѡ��һ��presentģʽ(����ͼ�񽻻���ģʽ)
        VkPresentModeKHR presentMode = ChooseSwapPresentMode(swapChainSupport.presentModes);
        // ѡ��һ�����ʵ�ͼ��ֱ���
        VkExtent2D extent = ChooseSwapExtent(swapChainSupport.capabilities);

        // �������е�ͼ���������������Ϊ�������еĳ��ȡ���ָ������ʱͼ�����С���������ǽ����Դ���1��ͼ����������ʵ�����ػ��塣
        uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
        // ���maxImageCount����0���ͱ�ʾû�����ơ��������0�ͱ�ʾ�����ƣ���ô�������ֻ�����õ�maxImageCount
        if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
            imageCount = swapChainSupport.capabilities.maxImageCount;

        // �����������Ľṹ��
        VkSwapchainCreateInfoKHR createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        // �����ǵ�surface
        createInfo.surface = surface;
        // ��ǰ���ȡ����������
        createInfo.minImageCount = imageCount;
        createInfo.imageFormat = surfaceFormat.format;
        createInfo.imageColorSpace = surfaceFormat.colorSpace;
        createInfo.presentMode = presentMode;
        createInfo.imageExtent = extent;

        // imageArrayLayersָ��ÿ��ͼ���ж��ٲ㣬һ�㶼��1
        createInfo.imageArrayLayers = 1;
        // ����ֶ�ָ���ڽ������ж�ͼ����еľ������
        // �������ֱ�Ӷ����ǽ�����Ⱦ������ζ��������Ϊ��ɫ����
        // Ҳ�������Ƚ�ͼ����ȾΪ������ͼ�񣬽��к������
        // ����������¿���ʹ����VK_IMAGE_USAGE_TRANSFER_DST_BIT������ֵ����ʹ���ڴ��������Ⱦ��ͼ���䵽������ͼ�����
        createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        // ָ����δ���������дصĽ�����ͼ��
        uint32_t tmpQueueFamilyIndices[] = { queueFamilyIndices.graphics, queueFamilyIndices.present };
        // ���graphics���д���presentation���дز�ͬ���������������
        // ���ǽ���graphics�����л��ƽ�������ͼ��Ȼ������һ��presentation�������ύ����
        // ����д���ͼ�������ַ���
        // VK_SHARING_MODE_EXCLUSIVE: ͬһʱ��ͼ��ֻ�ܱ�һ�����д�ռ�ã�����������д���Ҫ������Ȩ��Ҫ��ȷָ�������ַ�ʽ�ṩ����õ�����
        // VK_SHARING_MODE_CONCURRENT: ͼ����Ա�������дط��ʣ�����Ҫ��ȷ����Ȩ������ϵ
        // ������дز�ͬ����ʱʹ��concurrentģʽ�����⴦��ͼ������Ȩ������ϵ�����ݣ���Ϊ��Щ���漰���ٸ���
        if (queueFamilyIndices.graphics != queueFamilyIndices.present)
        {
            createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            // Concurrentģʽ��ҪԤ��ָ�����д�����Ȩ������ϵ��ͨ��queueFamilyIndexCount��pQueueFamilyIndices�������й���
            createInfo.queueFamilyIndexCount = 2;
            createInfo.pQueueFamilyIndices = tmpQueueFamilyIndices;
        }
        // ���graphics���дغ�presentation���д���ͬ��������Ҫʹ��exclusiveģʽ����Ϊconcurrentģʽ��Ҫ����������ͬ�Ķ��д�
        else
        {
            createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
            createInfo.queueFamilyIndexCount = 0; // Optional
            createInfo.pQueueFamilyIndices = nullptr; // Optional
        }

        // ���������֧��(supportedTransforms in capabilities)�����ǿ���Ϊ������ͼ��ָ��ĳЩת���߼�
        // ����90��˳ʱ����ת����ˮƽ��ת���������Ҫ�κ�transoform���������Լ򵥵�����ΪcurrentTransoform
        createInfo.preTransform = swapChainSupport.capabilities.currentTransform;

        // compositeAlpha�ֶ�ָ��alphaͨ���Ƿ�Ӧ�����������Ĵ���ϵͳ���л�ϲ�����������Ըù��ܣ��򵥵���VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR
        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

        // ���clipped��Ա����ΪVK_TRUE����ζ�����ǲ����ı��ڱε��������ݣ��������������Ĵ�������ǰ��ʱ������Ⱦ�Ĳ������ݴ����ڿ�������֮��
        // ���������Ҫ��ȡ��Щ���ػ����ݽ��д���������Կ����ü�����������
        createInfo.clipped = VK_TRUE;

        // Vulkan����ʱ��������������ĳЩ�����±��滻�����細�ڵ�����С���߽�������Ҫ���·�������ͼ�����
        // ����������£�������ʵ������Ҫ���·��䴴���������ڴ��ֶ���ָ���Ծɵ����ã����Ի�����Դ
        // Ҳ�����Լ��ֶ����پɽ����������´�������ʹ���������
        // ����������������һ�㣬��������ھɽ���������չʾ��ʱ���Ȼ��ƺ���ͼ���½�������������Ҫ�Ⱦɽ���������ִ���꣬�����٣������µģ��ٻ���
        createInfo.oldSwapchain = VK_NULL_HANDLE;

        // ����������
        if (vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapChain) != VK_SUCCESS) {
            throw std::runtime_error("failed to create swap chain!");
        }

        // ��ȡһ�½�������ͼ������������imageCount�����ʼ������������ͼ������������ʵ�ʴ����Ĳ�һ������ô�࣬����Ҫ���»�ȡһ��
        vkGetSwapchainImagesKHR(device, swapChain, &imageCount, nullptr);
        // ����ʵ��ͼ��������������vector��С
        swapChainImages.resize(imageCount);
        // �ѽ������ϵ�ͼ��洢��swapChainImages��
        vkGetSwapchainImagesKHR(device, swapChain, &imageCount, swapChainImages.data());
        // ��һ�½�����ͼ��ĸ�ʽ�ͷֱ���
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
            // layer��ָ��ͼ�������еĲ�����������ͼ���ǵ���ͼ����˲���Ϊ1
            framebufferInfo.layers = 1;

            if (vkCreateFramebuffer(device, &framebufferInfo, nullptr, &vulkanFBO->frameBuffers[i]) != VK_SUCCESS)
                throw std::runtime_error("failed to create framebuffer!");
        }

        vulkanFBO->inUse = true;
    }


    // ----------------------------------------����Vulkan����ĸ�������----------------------------------------


    bool RenderAPIVulkan::CheckValidationLayerSupport()
    {
        // ��ȡ���п��õ�Layer����
        uint32_t layerCount = 0;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

        // ��ȡ���п���Layer������
        vector<VkLayerProperties> availableLayers(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

        // ������ǵ�validationLayers�е�����layer�Ƿ������availableLayers�б���
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
        // Vulakn����ƽ̨��������API֧�ֵ�(������ʱ����)������ζ����Ҫһ����չ�����벻ͬƽ̨�Ĵ���ϵͳ���н���
        // GLFW��һ����������ú������������йص���չ��Ϣ
        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions;
        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        vector<const char*> extensions;

        // ���GLFW��ȡ����չ
        for (unsigned int i = 0; i < glfwExtensionCount; i++)
            extensions.push_back(glfwExtensions[i]);

        // ���������Debug�����Debug����չ
        if (validationLayersEnabled)
            extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

        return extensions;
    }

    // Vulkan��Debug����չ���ܣ�����vkCreateDebugUtilsMessengerEXT�������Զ�����
    // ���������Ҫͨ��vkGetInstanceProcAddr�ֶ���ȡ������ַ
    // ����������Ƿ�װ��һ�»�ȡvkCreateDebugUtilsMessengerEXT��ַ���������Ĺ���
    VkResult RenderAPIVulkan::CreateDebugUtilsMessengerEXT(const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger)
    {
        auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(vkInstance, "vkCreateDebugUtilsMessengerEXT");
        if (func != nullptr)
            return func(vkInstance, pCreateInfo, pAllocator, pDebugMessenger);
        else
            return VK_ERROR_EXTENSION_NOT_PRESENT;
    }

    // ���������ĺ���������VkDebugUtilsMessengerEXT��������Ҫ�ֶ�����vkDestroyDebugUtilsMessengerEXT����
    void RenderAPIVulkan::DestroyDebugUtilsMessengerEXT(VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator)
    {
        auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(vkInstance, "vkDestroyDebugUtilsMessengerEXT");
        if (func != nullptr)
            func(vkInstance, debugMessenger, pAllocator);
    }

    bool RenderAPIVulkan::IsPhysicalDeviceSuitable(VkPhysicalDevice device)
    {
        // ���name, type�Լ�Vulkan�汾�Ȼ������豸����
        VkPhysicalDeviceProperties deviceProperties;
        vkGetPhysicalDeviceProperties(device, &deviceProperties);
        // ��Ҫ����
        if (deviceProperties.deviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
            return false;

        // ��ѯ������ѹ����64λ�������Ͷ���ͼ��Ⱦ(VR�ǳ�����)�ȿ�ѡ���ܵ�֧��
        VkPhysicalDeviceFeatures deviceFeatures;
        vkGetPhysicalDeviceFeatures(device, &deviceFeatures);
        // ��Ҫ֧�ּ�����ɫ��
        if (!deviceFeatures.geometryShader)
            return false;

        // ��Ҫ֧�ָ������Բ���
        if (!deviceFeatures.samplerAnisotropy)
            return false;

        // ��ѯ���������Ķ��д�
        QueueFamilyIndices indices = GetQueueFamilyIndices(device);
        if (!indices.isComplete())
            return false;

        // ����Ƿ�֧������Ҫ����չ
        if (!CheckDeviceExtensionSupport(device))
            return false;

        // ��齻�����Ƿ�����
        SwapChainSupportDetails swapChainSupport = GetSwapChainSupportDetails(device);
        // ����֧��һ��ͼ���ʽ��һ��Presentģʽ
        bool swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
        if (!swapChainAdequate)
            return false;

        // ����кܶ��Կ�������ͨ��������Ӳ������һ��Ȩ�أ�Ȼ������ѡ������ʵ�
        return true;
    }

    QueueFamilyIndices RenderAPIVulkan::GetQueueFamilyIndices(VkPhysicalDevice device)
    {
        QueueFamilyIndices indices;

        // ��ȡ�豸ӵ�еĶ��д�����
        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

        // ��ȡ���ж��дػ������
        vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

        // �������дأ���ȡ֧����������Ķ��д�
        uint32_t i = 0;
        for (const auto& queueFamily : queueFamilies)
        {
            // ��ǰ���д��Ƿ�֧��ͼ�δ���
            if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
                indices.graphics = i;

            // �Ƿ�֧��VkSurfaceKHR
            VkBool32 presentSupport = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);
            // ֧�ֵĻ���¼һ������
            if (queueFamily.queueCount > 0 && presentSupport)
                indices.present = i;

            // ע������֧��surface��graphic�Ķ��дز�һ����ͬһ��
            // ����ʹ����Щ���дص��߼���Ҫô�̶�����֧��surface��graphic�Ķ��д���������ͬ��������(���������ǲ���ͬһ�����������)
            // Ҫô������Ҷ��дص�ʱ����ȷָ������ͬʱ֧��surface��graphic��Ȼ�󱣴�ͬʱ֧��������Ҫ��Ķ��д�����(���ܿ��ܻ�õ�)

            if (indices.isComplete())
                break;

            i++;
        }

        // ���ؾ߱�ͼ�������Ķ���
        return indices;
    }

    bool RenderAPIVulkan::CheckDeviceExtensionSupport(VkPhysicalDevice device) {
        // ��ȡ�����豸֧�ֵ���չ����
        uint32_t extensionCount;
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

        // ��ȡ��֧�ֵľ�����Ϣ
        vector<VkExtensionProperties> availableExtensions(extensionCount);
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

        // �������Զ����deviceExtensionsת����set���ݽṹ(Ϊ�˱������2��forѭ����erase��ͬʱҲ���Ķ�ԭ����)
        set<string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());
        // ���������豸��֧�ֵ���չ�������������Ҫ����չ������ɾ��
        for (const auto& extension : availableExtensions)
            requiredExtensions.erase(extension.extensionName);

        // ���ȫ��ɾ�����ˣ�˵����������Ҫ����չ����֧�ֵģ�����˵������һЩ������Ҫ����չ����֧��
        return requiredExtensions.empty();
    }

    SwapChainSupportDetails RenderAPIVulkan::GetSwapChainSupportDetails(VkPhysicalDevice device)
    {
        SwapChainSupportDetails details;

        // ����VkPhysicalDevice��VkSurfaceKHR��ȡ��֧�ֵľ��幦�ܣ������������ǽ������ĺ������
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

        // ��ѯ֧�ֵ�surface��ʽ����Щ
        uint32_t formatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);
        if (formatCount != 0)
        {
            // ����ʽ����
            details.formats.resize(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
        }

        // ��ѯ֧�ֵ�surfaceģʽ����Щ
        uint32_t presentModeCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);
        if (presentModeCount != 0) {
            details.presentModes.resize(presentModeCount);
            // ���ģʽ����
            vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
        }

        return details;
    }

    void RenderAPIVulkan::GetPhysicalDeviceProperties()
    {
        VkPhysicalDeviceProperties physicalDeviceProperties;
        vkGetPhysicalDeviceProperties(physicalDevice, &physicalDeviceProperties);

        // ȡͬʱ֧��Color��Depth���������
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
        // ��һ����û����������ĸ�ʽ��ɫ�ʿռ���ϣ����������
        for (const auto& availableFormat : availableFormats)
        {
            if (availableFormat.format == VK_FORMAT_R8G8B8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
                return availableFormat;
        }

        // ���û������ĸ�ʽ������ֱ��ѡ���һ����ʽ
        // ��ʵ���ʱ������Ҳ���Ա���һ��availableFormats���Լ�дһ��������һ����ԽϺõĳ���
        return availableFormats[0];
    }

    VkPresentModeKHR RenderAPIVulkan::ChooseSwapPresentMode(const vector<VkPresentModeKHR> availablePresentModes) {
        // ��Vulkan�����ĸ�ģʽ����ʹ��:
        // 1��VK_PRESENT_MODE_IMMEDIATE_KHR
        // Ӧ�ó����ύ��ͼ���������䵽��Ļ���֣�����ģʽ���ܻ����˺��Ч����
        // 2��VK_PRESENT_MODE_FIFO_KHR
        // ������������һ�����У�����ʾ������Ҫˢ�µ�ʱ����ʾ�豸�Ӷ��е�ǰ���ȡͼ�񣬲��ҳ�����Ⱦ��ɵ�ͼ�������еĺ��档������������ĳ����ȴ������ֹ�ģ����Ƶ��Ϸ�Ĵ�ֱͬ�������ơ���ʾ�豸��ˢ��ʱ�̱���Ϊ����ֱ�жϡ���
        // 3��VK_PRESENT_MODE_FIFO_RELAXED_KHR
        // ��ģʽ����һ��ģʽ���в�ͬ�ĵط�Ϊ�����Ӧ�ó�������ӳ٣����������һ����ֱͬ���ź�ʱ���п��ˣ�������ȴ���һ����ֱͬ���źţ����ǽ�ͼ��ֱ�Ӵ��͡����������ܵ��¿ɼ���˺��Ч����
        // 4��VK_PRESENT_MODE_MAILBOX_KHR
        // ���ǵڶ���ģʽ�ı��֡�����������������ʱ��ѡ���µ��滻�ɵ�ͼ�񣬴Ӷ��������Ӧ�ó�������Ρ�����ģʽͨ������ʵ�����ػ����������׼�Ĵ�ֱͬ��˫������ȣ���������Ч�����ӳٴ�����˺��Ч����

        // Ĭ�ϵ�ģʽ
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
        // ���capabilities������ͨ��vkGetPhysicalDeviceSurfaceCapabilitiesKHR�ӿڲ�ѯ������

        // currentExtent�ĸ߿������һ�������uint32���ֵ����˵�����������Լ���һ����Χ�������������ֵ
        if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
        {
            int width, height;
            glfwGetFramebufferSize(RenderEngine::GetInstance()->window, &width, &height);

            VkExtent2D actualExtent =
            {
                static_cast<uint32_t>(width),
                static_cast<uint32_t>(height)
            };

            // ������minImageExtent��maxImageExtent֮��
            actualExtent.width = Math::Max(capabilities.minImageExtent.width, Math::Min(capabilities.maxImageExtent.width, actualExtent.width));
            actualExtent.height = Math::Max(capabilities.minImageExtent.height, Math::Min(capabilities.maxImageExtent.height, actualExtent.height));

            return actualExtent;
        }
        // ��������ֻ��ʹ�ò�ѯ����currentExtent
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
        // ��Uniform Buffer�Ĵ�С��minUniformBufferOffsetAlignment���룬�Ҳ�̫ȷ����������ǲ��Ǳ�Ҫ��
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
        // ����ʱ��������Ϊsignaled״̬(�����һ�ε�vkWaitForFences��Զ�Ȳ������)
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
        // ���������ά�ȵ������������������3D����Ӧ�ö���1
        imageInfo.extent.depth = 1;
        imageInfo.mipLevels = mipLevels;
        imageInfo.arrayLayers = layers;
        imageInfo.format = format;
        // VK_IMAGE_TILING_LINEAR: texel����Ϊ��������Ϊ����
        // VK_IMAGE_TILING_OPTIMAL: texel����Vulkan�ľ���ʵ���������һ��˳�����У���ʵ����ѷ���
        // �����layout��һ����һ������֮���ǹ̶��Ĳ��ܸģ����CPU��Ҫ��ȡ������ݣ�������ΪVK_IMAGE_TILING_LINEAR
        // ���ֻ��GPUʹ�ã�������ΪVK_IMAGE_TILING_OPTIMAL���ܸ���
        imageInfo.tiling = tiling;
        // ����ֻ����VK_IMAGE_LAYOUT_UNDEFINED����VK_IMAGE_LAYOUT_PREINITIALIZED
        // VK_IMAGE_LAYOUT_UNDEFINED��ζ�ŵ�һ��transition���ݵ�ʱ�����ݻᱻ����
        // VK_IMAGE_LAYOUT_PREINITIALIZED�ǵ�һ��transition���ݵ�ʱ�����ݻᱻ����
        // ���Ǻܶ����ʲô��˼�������һ��������CPUд�����ݣ�Ȼ��transfer������VkImage��stagingImage����Ҫ��VK_IMAGE_LAYOUT_PREINITIALIZED
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageInfo.usage = usage;
        imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        // ���ֻӰ�쵱��attachmentsʹ�õ�VkImage(�Լ�������frame buffer��֧��������������õ��Ǹ�Ĭ��buffer��֧��)
        imageInfo.samples = numSamples;
        // ���Լ�һЩ��־����������;��ͼ�����Ż�������3D��ϡ��(sparse)ͼ��
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
        // ����mipmap��2�ַ�ʽ
        // һ������һЩ�ⲿ�Ľӿڣ�����stb_image_resize��ȥ����ÿһ���ͼ�����ݣ�Ȼ���ÿһ�㶼��ԭʼͼ��������������
        // ������������һ�ַ�ʽ����vkCmdBlitImage��������������ڸ��ƣ����ź�filterͼ�����ݵ�
        // ��һ��ѭ�����level 0(ԭͼ)������Сһ��blit��level 1��Ȼ��1��2��2��3����

        // �ȼ��ͼ���ʽ�Ƿ�֧��linear blitting
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
            // ע��ѭ���Ǵ�1��ʼ��
            for (uint32_t i = 1; i < mipLevels; i++)
            {
                // �Ȱѵ�i-1��(0��ԭͼ)��layoutת��TRANSFER_SRC_OPTIMAL
                barrier.subresourceRange.baseMipLevel = i - 1;
                // ԭlayout����ʵimageһ��ʼ������ʱ��ÿһ��mipmap������ΪVK_IMAGE_LAYOUT_UNDEFINED��
                // ����ÿһ��mipmap������ΪĿ��ͼ��������ݣ�����Ϊԭͼ������һ����������
                // ���������i-1��mipmap���൱������һ��Blit������ԭ���ݣ�Ҳ�����ѭ������ڶ��α�ʹ��(��һ�α�ʹ������ΪĿ��ͼ��)
                // ��������Ҫ��TRANSFER_DST_OPTIMALת����TRANSFER_SRC_OPTIMAL
                barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
                barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
                // ���image������д��Ӧ�������Barrier֮ǰ���
                barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
                // ���Barrier֮��Ϳ��Զ����image��������
                barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

                vkCmdPipelineBarrier(cmd,
                    // ָ��Ӧ����Barrier֮ǰ��ɵĲ������ڹ�������ĸ�stage
                    VK_PIPELINE_STAGE_TRANSFER_BIT,
                    // ָ��Ӧ�õȴ�Barrier�Ĳ������ڹ�������ĸ�stage
                    VK_PIPELINE_STAGE_TRANSFER_BIT,
                    0, 0, nullptr, 0, nullptr, 1, &barrier
                );

                // ����Blit����������Blit�������ǰ�ͬһ��image��i-1��mipmap��������Сһ�븴�Ƶ���i��
                VkImageBlit blit{};
                // ����ԭͼ���(0,0)��(width, height)
                blit.srcOffsets[0] = { 0, 0, 0 };
                blit.srcOffsets[1] = { mipWidth, mipHeight, 1 };
                // ����ԭͼ���Color
                blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                // ����ԭͼ��mipmap�ȼ���i-1
                blit.srcSubresource.mipLevel = i - 1;
                // ��ʱû��
                blit.srcSubresource.baseArrayLayer = 0;
                blit.srcSubresource.layerCount = 1;
                // ���Ƶ�Ŀ��ͼ���(0,0)��(width/2, height/2)�����С��1�Ļ�����1
                blit.dstOffsets[0] = { 0, 0, 0 };
                blit.dstOffsets[1] = { mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1 };
                // ����Ŀ��ͼ���Color
                blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                // ���Ƶ�Ŀ��ͼ���mipmap�ȼ�i
                blit.dstSubresource.mipLevel = i;
                // ��ʱû��
                blit.dstSubresource.baseArrayLayer = 0;
                blit.dstSubresource.layerCount = 1;

                // ���Bilt����ָ�����ԭͼ���Ŀ��ͼ������Ϊͬһ������Ϊ��ͬһ��image�Ĳ�ͬmipmap�����
                vkCmdBlitImage(cmd,
                    image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                    image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                    1, &blit, VK_FILTER_LINEAR
                );

                // Blit��֮�����Barrier����Ӧ��i-1��mipmap�ͽ��������ˣ������ṩ��shader��ȡ��
                // ����layout��TRANSFER_SRC_OPTIMALת����SHADER_READ_ONLY_OPTIMAL
                barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
                barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                // ���image��i-1��mipmap�����ݶ�ȡ����Ӧ�������Barrier֮ǰ���
                barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
                barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

                vkCmdPipelineBarrier(cmd,
                    // ���ǰ���srcAccessMask
                    // transfer�׶ε�transfer��ȡ����Ӧ�������Barrier֮ǰִ��
                    VK_PIPELINE_STAGE_TRANSFER_BIT,
                    // ���ǰ���dstAccessMask
                    // fragment shader�׶ε�shader��ȡ����Ӧ�������Barrier֮��ִ��
                    VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                    0, 0, nullptr, 0, nullptr, 1, &barrier
                );

                if (mipWidth > 1) mipWidth /= 2;
                if (mipHeight > 1) mipHeight /= 2;
            }

            // ѭ�������������һ����mipmap��Ҫ����
            barrier.subresourceRange.baseMipLevel = mipLevels - 1;
            // ��Ϊ���һ��ֻ�������ݣ�����Ҫ�����������ݵ������ط�����������layout����TRANSFER_DST_OPTIMAL
            barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            // ��Ҫת����shader��ȡ�õ�SHADER_READ_ONLY_OPTIMAL
            barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            // ���Barrier֮ǰ��Ҫ������һ��mipmap������д��
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            // shader��ȡ������Ҫ�����Barrier֮����ܿ�ʼ
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
            
            vkCmdPipelineBarrier(cmd,
                // ���ǰ���srcAccessMask
                // transfer�׶ε�transferд�����Ӧ�������Barrier֮ǰִ��
                VK_PIPELINE_STAGE_TRANSFER_BIT,
                // ���ǰ���dstAccessMask
                // fragment shader�׶εĶ�ȡ������Ҫ�����Barrier֮����ܿ�ʼ
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

        // components�ֶ����������ɫͨ�������յ�ӳ���߼�
        // ���磬���ǿ��Խ�������ɫͨ��ӳ��Ϊ��ɫͨ������ʵ�ֵ�ɫ��������Ҳ���Խ�ͨ��ӳ�����ĳ�����ֵ0��1
        // ������Ĭ�ϵ�
        createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

        // subresourceRangle�ֶ���������ͼ���ʹ��Ŀ����ʲô���Լ����Ա����ʵ���Ч����
        // ���ͼ���������color����depth stencil��
        createInfo.subresourceRange.aspectMask = aspectFlags;
        // Ĭ�ϴ�������Mipmap
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = VK_REMAINING_MIP_LEVELS;
        // Ĭ�ϴ�������Layers
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
        // ������������filter
        samplerInfo.anisotropyEnable = VK_TRUE;
        samplerInfo.maxAnisotropy = maxSamplerAnisotropy;
        samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        // ������false������������귶Χ����������[0, 1)�������true���ͻ���[0, texWidth)��[0, texHeight)�����󲿷�����¶�����[0, 1)
        samplerInfo.unnormalizedCoordinates = VK_FALSE;
        // һ���ò��������ĳЩ����������shadow map��percentage-closer filtering���õ�
        samplerInfo.compareEnable = VK_FALSE;
        samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
        // mipmap����
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
            // �����Ǹ�����������color��depth�ģ�stencil�ĵ���һ��
            colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            // ��Ⱦ��ʼǰ�ͽ�����Layout���Ǹ�Shader��ȡ����׼���ģ�ֻ����Ⱦ�иĳ�Attachment��Ҫ��Layout
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
            // ָ��color buffer�����ã���������Լ���������(�����attachment = 0)ֱ�Ӷ�Ӧ��ƬԪ��ɫ�����layout(location = 0) out vec4 outColor
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
            // ǰ�洴��VkAttachmentReference��ʱ���Ǹ�����attachmentָ�ľ��������pAttachments�����������
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
            // ǰ�洴��VkAttachmentReference��ʱ���Ǹ�����attachmentָ�ľ��������pAttachments�����������
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
            // ǰ�洴��VkAttachmentReference��ʱ���Ǹ�����attachmentָ�ľ��������pAttachments�����������
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

        // ���ö��������ʽ
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

        // ����ͼԪ
        VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo = GetAssemblyInfo(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);

        // ViewPort��Ϣ�����ﲻֱ�����ã�����Ū�ɶ�̬��
        VkPipelineViewportStateCreateInfo viewportStateInfo = {};
        viewportStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportStateInfo.viewportCount = 1;
        viewportStateInfo.scissorCount = 1;

        // View Port��Scissor����Ϊ��̬��ÿ֡����ʱ����
        vector<VkDynamicState> dynamicStates = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
        VkPipelineDynamicStateCreateInfo dynamicStateInfo = {};
        dynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamicStateInfo.pDynamicStates = dynamicStates.data();
        dynamicStateInfo.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());

        // ���ù�դ���׶�
        VkPipelineRasterizationStateCreateInfo rasterizationInfo = GetRasterizationInfo(vkFaceCullOptionMap[shaderInfo.stateSet.cull]);
        
        // ����Shader���������MSAA(�����������Ļ�ϵ�MSAA)����Ҫ�����߼��豸��ʱ����VkPhysicalDeviceFeatures���sampleRateShading������Ч����ʱ�ر�
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

        // ��Ⱥ�ģ������
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
        // ������Ҫȷ����������uint32_t�Ķ���Ҫ��,�洢��vector�У�Ĭ�Ϸ������Ѿ�ȷ�����������������µĶ���Ҫ��
        createInfo.codeSize = code.size();
        // ת��ΪVulkanҪ���uint32_tָ��
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
        // ����mipmap�ȼ�
        // ͨ����ͼƬ�߿���Сһ�����һ����ֱ��������
        // ��max�ҳ��߿�������Ƚϴ�ģ�Ȼ����log2������Ա�2�����Σ�������ȡ����������ͼ������С���ٴ���
        // ����1����ΪԭͼҲҪһ���ȼ�
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
        // ����ʱ��������Ϊsignaled״̬(�����һ����Զ�Ȳ���)
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
            // ����������������ת�����д�����Ȩ�ģ�������ǲ������ת����һ��Ҫ��ȷ����VK_QUEUE_FAMILY_IGNORED
            barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            barrier.image = image;
            // Ĭ�ϴ�������mipmap
            barrier.subresourceRange.baseMipLevel = 0;
            barrier.subresourceRange.levelCount = VK_REMAINING_MIP_LEVELS;
            // Ĭ�ϴ�������Layer
            barrier.subresourceRange.baseArrayLayer = 0;
            barrier.subresourceRange.layerCount = VK_REMAINING_ARRAY_LAYERS;
            // Image��;(Color, Depth, Stencil)
            barrier.subresourceRange.aspectMask = aspectMask;
            barrier.srcAccessMask = srcAccessMask;
            barrier.dstAccessMask = dstAccessMask;

            vkCmdPipelineBarrier(cmd, srcStage, dstStage,
                // ���������0����VK_DEPENDENCY_BY_REGION_BIT��������ζ�������ȡ��ĿǰΪֹ��д�����Դ����
                0,
                // VkMemoryBarrier����
                0, nullptr,
                // VkBufferMemoryBarrier����
                0, nullptr,
                // VkImageMemoryBarrier����
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
        // ���depthClampEnable����ΪVK_TRUE������Զ���ü����ƬԪ����������������Ƕ�������
        rasterizationInfo.depthClampEnable = VK_FALSE;
        // ���rasterizerDiscardEnable����ΪVK_TRUE����ô����ͼԪ��Զ���ᴫ�ݵ���դ���׶�
        // ���ǽ�ֹ�κ����������framebuffer�ķ���
        rasterizationInfo.rasterizerDiscardEnable = VK_FALSE;
        // ����ƬԪ��δӼ���ģ���в������������FILL����Ҫ����GPU feature
        // VK_POLYGON_MODE_FILL: ������������
        // VK_POLYGON_MODE_LINE: ����α�Ե�߿����
        // VK_POLYGON_MODE_POINT : ����ζ�����Ϊ������
        rasterizationInfo.polygonMode = VK_POLYGON_MODE_FILL;
        rasterizationInfo.lineWidth = 1.0f;
        rasterizationInfo.cullMode = cullMode;
        rasterizationInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
        // ��Ⱦ��Ӱ��ƫ������
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
        // ����ǵ���sampleShadingЧ���ģ�Խ�ӽ�1Ч��Խƽ����Խ�ӽ�0����Խ��
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