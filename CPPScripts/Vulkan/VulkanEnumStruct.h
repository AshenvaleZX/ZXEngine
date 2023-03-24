#pragma once
#include "../pubh.h"
// ��GLFW�Ļ�����Ͳ�Ҫ�Լ�ȥinclude Vulkan��ͷ�ļ���������궨�壬��GLFW�Լ�ȥ��������Ȼ��Щ�ӿ�������
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include "vk_mem_alloc.h"

#define ShaderModuleSet map<VkShaderStageFlagBits, VkShaderModule>

namespace ZXEngine
{
    // ��GPU��Ⱦ�����ʱ��CPU���Դ�����֡��
    const uint32_t MAX_FRAMES_IN_FLIGHT = 2;

    // ��Ҫ����֤��
    const vector<const char*> validationLayers =
    {
        // Vulkan SDKͨ������VK_LAYER_KHRONOS_validation�㣬����ʽ�Ŀ�������������ϵ�layers���Ӷ�������ȷ��ָ�����е���ȷ����ϲ�
        "VK_LAYER_KHRONOS_validation"
    };

    // ��Ҫ�õ�����չ
    const vector<const char*> deviceExtensions =
    {
        // ��������չ���������֧��Ҳ�ʹ������Ƿ�֧�ֽ�ͼ����Ƶ���ʾ����(��������GPU������������ͼ)
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    enum class RenderPassType
    {
        Present, // For swap chain
        Normal,
        MAX,
    };

    struct QueueFamilyIndices
    {
        int graphicsFamilyIdx = -1;
        int presentFamilyIdx = -1;

        bool isComplete() { return graphicsFamilyIdx >= 0 && presentFamilyIdx >= 0; }
    };

    // ����������������������
    struct SwapChainSupportDetails
    {
        // ������surface��������(min/max number of images in swap chain, min/max width and height of images)
        VkSurfaceCapabilitiesKHR capabilities = {};
        // Surface��ʽ(pixel format, color space)
        vector<VkSurfaceFormatKHR> formats;
        // ��Ч��presentationģʽ
        vector<VkPresentModeKHR> presentModes;
    };

    struct VulkanBuffer
    {
        VkBuffer buffer = VK_NULL_HANDLE;
        VmaAllocation allocation = VK_NULL_HANDLE;
    };

    struct UniformBuffer
    {
        uint32_t binding = 0;
        VkDeviceSize size = 0;
        VulkanBuffer buffer;
        void* mappedAddress;
    };

    struct VulkanImage
    {
        VkImage image = VK_NULL_HANDLE;
        VmaAllocation allocation = VK_NULL_HANDLE;
    };

    struct VulkanTexture
    {
        VulkanImage image;
        VkImageView imageView = VK_NULL_HANDLE;
        VkSampler sampler = VK_NULL_HANDLE;
        bool inUse = false;
    };

    struct VulkanFBO
    {
        vector<VkFramebuffer> frameBuffers;
        FrameBufferType bufferType = FrameBufferType::Normal;
        RenderPassType renderPassType = RenderPassType::Normal;
        bool inUse = false;
    };

    struct VulkanAttachmentBuffer
    {
        vector<uint32_t> attachmentBuffers;
        bool inUse = false;
    };

    struct VulkanDrawCommand
    {
        vector<VkCommandBuffer> commandBuffers;
        bool inUse = false;
    };

    struct VulkanVAO
    {
        VkBuffer indexBuffer = VK_NULL_HANDLE;
        VmaAllocation indexBufferAlloc = VK_NULL_HANDLE;
        void* indexBufferAddress = nullptr; // Only for dynamic mesh
        VkBuffer vertexBuffer = VK_NULL_HANDLE;
        VmaAllocation vertexBufferAlloc = VK_NULL_HANDLE;
        void* vertexBufferAddress = nullptr; // Only for dynamic mesh
        uint32_t size = 0;
        bool inUse = false;
    };

    struct VulkanPipeline
    {
        VkPipeline pipeline;
        VkPipelineLayout pipelineLayout;
        VkDescriptorSetLayout descriptorSetLayout;
        VkDescriptorPool descriptorPool;
        vector<VkDescriptorSet> descriptorSets;
        vector<UniformBuffer> vertUniformBuffers;
        vector<UniformBuffer> geomUniformBuffers;
        vector<UniformBuffer> fragUniformBuffers;
        bool inUse = false;
    };
}