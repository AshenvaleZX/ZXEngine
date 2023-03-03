#pragma once
#include "../pubh.h"
// ��GLFW�Ļ�����Ͳ�Ҫ�Լ�ȥinclude Vulkan��ͷ�ļ���������궨�壬��GLFW�Լ�ȥ������Ȼ��Щ�ӿ�������
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include "vk_mem_alloc.h"

#define ShaderModuleSet map<VkShaderStageFlagBits, VkShaderModule>

namespace ZXEngine
{
    // ��GPU��Ⱦ�����ʱ��CPU���Դ����֡��
    const uint32_t MAX_FRAMES_IN_FLIGHT = 2;

    enum class RenderPassType
    {
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

    struct VulkanVAO
    {
        VkBuffer indexBuffer = VK_NULL_HANDLE;
        VmaAllocation indexBufferAlloc = VK_NULL_HANDLE;
        VkBuffer vertexBuffer = VK_NULL_HANDLE;
        VmaAllocation vertexBufferAlloc = VK_NULL_HANDLE;
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