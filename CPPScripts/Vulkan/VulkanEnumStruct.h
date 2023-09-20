#pragma once
#include "../pubh.h"
// ���volk�⣬���������OpenGL��GLAD�⣬����������Vulkan������
// ���������OpenGL��һ����������Ǳ���ģ����ֻ������դ�����ߵ���Ⱦ���Ͳ���Ҫ����Vulkan����չ���Ϳ��Բ����������
// ����Vulkan�Ĺ���׷����Ⱦģ����Vulkan��չ���Vulkan��չ��ĺ���ȫ����Ҫ�����ֶ����أ�������Ҫ�����������������Vulkan����
#define VK_NO_PROTOTYPES
#include <volk.h>
// ��GLFW�Ļ�����Ͳ�Ҫ�Լ�ȥinclude Vulkan��ͷ�ļ���������궨�壬��GLFW�Լ�ȥ������Ȼ��Щ�ӿ�������
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
// AMDд��Vulkan�ڴ������
#include "vk_mem_alloc.h"
#include "../PublicStruct.h"

#define ShaderModuleSet map<VkShaderStageFlagBits, VkShaderModule>

namespace ZXEngine
{
    // ��GPU��Ⱦ�����ʱ��CPU���Դ����֡��
    const uint32_t MAX_FRAMES_IN_FLIGHT = 1;

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
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
        // ��׷��չ
        VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME,
        VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME,
        VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME,
        // Shader��ʱ����չ
        VK_KHR_SHADER_CLOCK_EXTENSION_NAME
    };

    enum class RenderPassType
    {
        Present, // For swap chain
        Normal,
        Color,
        ShadowMap,
        ShadowCubeMap,
        MAX,
    };

    struct QueueFamilyIndices
    {
        uint32_t present = UINT32_MAX;
        uint32_t graphics = UINT32_MAX;

        bool isComplete() { return present != UINT32_MAX && graphics != UINT32_MAX; }
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
        void* mappedAddress = nullptr;
        VkDeviceAddress deviceAddress = 0;
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
        uint32_t colorAttachmentIdx = UINT32_MAX;
        uint32_t depthAttachmentIdx = UINT32_MAX;
        FrameBufferType bufferType = FrameBufferType::Normal;
        RenderPassType renderPassType = RenderPassType::Normal;
        ClearInfo clearInfo = {};
        bool inUse = false;
    };

    struct VulkanAttachmentBuffer
    {
        vector<uint32_t> attachmentBuffers;
        bool inUse = false;
    };

    struct VulkanDrawIndex
    {
        uint32_t VAO = 0;
        uint32_t pipelineID = 0;
        uint32_t materialDataID = 0;
    };

    // For build Vulkan Acceleration Structure Instance
    struct VulkanASInstanceData
    {
        uint32_t VAO = 0;
        uint32_t hitGroupIdx = 0;
        uint32_t rtMaterialDataID = 0;
        Matrix4 transform;
    };

    struct VulkanCommand
    {
        VkCommandBuffer commandBuffer = VK_NULL_HANDLE;
        vector<VkSemaphore> signalSemaphores;
    };

    struct VulkanDrawCommand
    {
        CommandType commandType = CommandType::NotCare;
        vector<VulkanCommand> drawCommands;
        bool inUse = false;
    };

    struct VulkanAccelerationStructure
    {
        bool isBuilt = false;
        VulkanBuffer buffer;
        VkDeviceAddress deviceAddress = 0;
        VkAccelerationStructureKHR as = VK_NULL_HANDLE;
    };

    struct VulkanASGroup
    {
        vector<VulkanAccelerationStructure> asGroup;
        bool inUse = false;
    };

    struct VulkanVAO
    {
        uint32_t indexCount = 0; // ��������
        VkBuffer indexBuffer = VK_NULL_HANDLE;
        VmaAllocation indexBufferAlloc = VK_NULL_HANDLE;
        void* indexBufferAddress = nullptr; // Only for dynamic mesh
        VkDeviceAddress indexBufferDeviceAddress = 0; // Only for ray tracing

        uint32_t vertexCount = 0; // ��������
        VkBuffer vertexBuffer = VK_NULL_HANDLE;
        VmaAllocation vertexBufferAlloc = VK_NULL_HANDLE;
        void* vertexBufferAddress = nullptr; // Only for dynamic mesh
        VkDeviceAddress vertexBufferDeviceAddress = 0; // Only for ray tracing

        VulkanAccelerationStructure blas; // Bottom Level Acceleration Structure
        bool inUse = false;
    };

    struct VulkanPipeline
    {
        string name; // For debug
        VkPipeline pipeline = VK_NULL_HANDLE;
        VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
        VkDescriptorSetLayout descriptorSetLayout = VK_NULL_HANDLE;
        VkDescriptorSetLayout sceneDescriptorSetLayout = VK_NULL_HANDLE; // For ray tracing
        bool inUse = false;
    };

    struct VulkanMaterialData
    {
        VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
        vector<VkDescriptorSet> descriptorSets;
        vector<UniformBuffer> vertUniformBuffers;
        vector<UniformBuffer> geomUniformBuffers;
        vector<UniformBuffer> fragUniformBuffers;
        bool inUse = false;
    };

    struct VulkanRTPipelineData
    {
        VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
        vector<VkDescriptorSet> descriptorSets;
    };

    struct VulkanShaderBindingTable
    {
        VulkanBuffer buffer;
        VkStridedDeviceAddressRegionKHR raygenRegion   = {};
        VkStridedDeviceAddressRegionKHR missRegion     = {};
        VkStridedDeviceAddressRegionKHR hitRegion      = {};
        VkStridedDeviceAddressRegionKHR callableRegion = {};
    };

    struct VulkanRTSceneData
    {
        VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
        vector<VkDescriptorSet> descriptorSets;
        vector<VulkanBuffer> dataReferenceBuffers;
    };

    struct VulkanRTPipeline
    {
        uint32_t tlasIdx = 0;
        VulkanPipeline pipeline;
        VulkanRTPipelineData pipelineData;
        VulkanShaderBindingTable SBT;
        VulkanRTSceneData sceneData;
    };
    
    struct VulkanRTMaterialData
    {
        vector<VulkanBuffer> buffers;
        bool inUse = false;
    };

    struct VulkanRTRendererDataReference
    {
        VkDeviceAddress indexAddress;
        VkDeviceAddress vertexAddress;
        VkDeviceAddress materialAddress;
    };
}