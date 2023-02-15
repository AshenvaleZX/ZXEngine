#pragma once
#include "pubh.h"
#include "vk_mem_alloc.h"

namespace ZXEngine
{
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
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    struct QueueFamilyIndices
    {
        int graphicsFamilyIdx = -1;
        int presentFamilyIdx = -1;

        bool isComplete()
        {
            return graphicsFamilyIdx >= 0 && presentFamilyIdx >= 0;
        }
    };

    // 交换链的三大类属性设置
    struct SwapChainSupportDetails {
        // 基本的surface功能属性(min/max number of images in swap chain, min/max width and height of images)
        VkSurfaceCapabilitiesKHR capabilities;
        // Surface格式(pixel format, color space)
        vector<VkSurfaceFormatKHR> formats;
        // 有效的presentation模式
        vector<VkPresentModeKHR> presentModes;
    };

    class RenderAPIVulkan
    {
    public:
        RenderAPIVulkan();
        ~RenderAPIVulkan() {};

    public:
        bool windowResized = false;

    private:
        // 是否开启验证层
        bool validationLayersEnabled = false;
        // MSAA采样点数量
        VkSampleCountFlagBits msaaSamplesCount = VK_SAMPLE_COUNT_1_BIT;

        // Vulkan实例
        VkInstance vkInstance = VK_NULL_HANDLE;
        // Debug工具
        VkDebugUtilsMessengerEXT debugMessenger = VK_NULL_HANDLE;
        // 物理设备
        VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
        // 逻辑设备
        VkDevice device = VK_NULL_HANDLE;
        // Vulkan Memory Allocator(来自AMD的GPUOpen团队:https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator)
        VmaAllocator vmaAllocator;
        // Surface
        VkSurfaceKHR surface = VK_NULL_HANDLE;
        // 图形队列
        VkQueue graphicsQueue = VK_NULL_HANDLE;
        // 展示队列
        VkQueue presentQueue = VK_NULL_HANDLE;
        // 交换链
        VkSwapchainKHR swapChain = VK_NULL_HANDLE;
        // 交换链上的图像(创建交换链的时候自动创建，销毁交换链的时候自动销毁)
        vector<VkImage> swapChainImages;
        // 交换链上图像的格式
        VkFormat swapChainImageFormat;
        // 交换链上图像的大小
        VkExtent2D swapChainExtent;
        // 命令池
        VkCommandPool commandPool = VK_NULL_HANDLE;

        // ------------------------------------------建立各种Vulkan对象--------------------------------------------

        void CreateVkInstance();
        void CreateDebugMessenger();
        void CreatePhysicalDevice();
        void CreateLogicalDevice();
        void CreateMemoryAllocator();
        void CreateSurface();
        void CreateSwapChain();

        // ----------------------------------------建立Vulkan对象的辅助函数----------------------------------------

        bool CheckValidationLayerSupport();
        vector<const char*> GetRequiredExtensions();
        // 创建调试用的VkDebugUtilsMessenger
        VkResult CreateDebugUtilsMessengerEXT(const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);
        // 销毁VkDebugUtilsMessenger
        void DestroyDebugUtilsMessengerEXT(VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);
        // 检查物理设备是否符合需求
        bool IsPhysicalDeviceSuitable(VkPhysicalDevice device);
        // 获取物理设备队列簇索引
        QueueFamilyIndices GetQueueFamilyIndices(VkPhysicalDevice device);
        // 检查物理设备是否支持所需要的扩展
        bool CheckDeviceExtensionSupport(VkPhysicalDevice device);
        // 获取物理设备所支持的交换链信息
        SwapChainSupportDetails GetSwapChainSupportDetails(VkPhysicalDevice device);
        // 获取硬件支持的最大超采样数量(一个像素几个采样点)
        VkSampleCountFlagBits GetMaxUsableSampleCount();
        // 选择交换链图像格式和色彩空间
        VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const vector<VkSurfaceFormatKHR>& availableFormats);
        // 选择交换链的展示模式
        VkPresentModeKHR ChooseSwapPresentMode(const vector<VkPresentModeKHR> availablePresentModes);
        // 选择交换链图像分辨率
        VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
    };
}