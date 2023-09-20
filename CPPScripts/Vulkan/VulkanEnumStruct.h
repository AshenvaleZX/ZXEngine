#pragma once
#include "../pubh.h"
// 添加volk库，这个库类似OpenGL的GLAD库，是用来加载Vulkan函数的
// 但是这个和OpenGL不一样，这个不是必须的，如果只是做光栅化管线的渲染，就不需要启用Vulkan的扩展，就可以不做这个加载
// 但是Vulkan的光线追踪渲染模块在Vulkan扩展里，而Vulkan扩展里的函数全部需要我们手动加载，所以需要用这个库来加载所有Vulkan函数
#define VK_NO_PROTOTYPES
#include <volk.h>
// 用GLFW的话这里就不要自己去include Vulkan的头文件，用这个宏定义，让GLFW自己去处理，不然有些接口有问题
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
// AMD写的Vulkan内存分配器
#include "vk_mem_alloc.h"
#include "../PublicStruct.h"

#define ShaderModuleSet map<VkShaderStageFlagBits, VkShaderModule>

namespace ZXEngine
{
    // 在GPU渲染画面的时候，CPU可以处理的帧数
    const uint32_t MAX_FRAMES_IN_FLIGHT = 1;

    // 需要的验证层
    const vector<const char*> validationLayers =
    {
        // Vulkan SDK通过请求VK_LAYER_KHRONOS_validation层，来隐式的开启有所关于诊断的layers，从而避免明确的指定所有的明确的诊断层
        "VK_LAYER_KHRONOS_validation"
    };

    // 需要用到的扩展
    const vector<const char*> deviceExtensions =
    {
        // 交换链扩展名，这个的支持也就代表了是否支持将图像绘制到显示器上(不是所有GPU都可以拿来绘图)
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
        // 光追扩展
        VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME,
        VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME,
        VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME,
        // Shader计时器扩展
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

    // 交换链的三大类属性设置
    struct SwapChainSupportDetails
    {
        // 基本的surface功能属性(min/max number of images in swap chain, min/max width and height of images)
        VkSurfaceCapabilitiesKHR capabilities = {};
        // Surface格式(pixel format, color space)
        vector<VkSurfaceFormatKHR> formats;
        // 有效的presentation模式
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
        uint32_t indexCount = 0; // 索引数量
        VkBuffer indexBuffer = VK_NULL_HANDLE;
        VmaAllocation indexBufferAlloc = VK_NULL_HANDLE;
        void* indexBufferAddress = nullptr; // Only for dynamic mesh
        VkDeviceAddress indexBufferDeviceAddress = 0; // Only for ray tracing

        uint32_t vertexCount = 0; // 顶点数量
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