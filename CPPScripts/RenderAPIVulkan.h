#pragma once
#include "pubh.h"
#include "vk_mem_alloc.h"

namespace ZXEngine
{
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

    struct QueueFamilyIndices
    {
        int graphicsFamilyIdx = -1;
        int presentFamilyIdx = -1;

        bool isComplete()
        {
            return graphicsFamilyIdx >= 0 && presentFamilyIdx >= 0;
        }
    };

    // ����������������������
    struct SwapChainSupportDetails {
        // ������surface��������(min/max number of images in swap chain, min/max width and height of images)
        VkSurfaceCapabilitiesKHR capabilities;
        // Surface��ʽ(pixel format, color space)
        vector<VkSurfaceFormatKHR> formats;
        // ��Ч��presentationģʽ
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
        // �Ƿ�����֤��
        bool validationLayersEnabled = false;
        // MSAA����������
        VkSampleCountFlagBits msaaSamplesCount = VK_SAMPLE_COUNT_1_BIT;

        // Vulkanʵ��
        VkInstance vkInstance = VK_NULL_HANDLE;
        // Debug����
        VkDebugUtilsMessengerEXT debugMessenger = VK_NULL_HANDLE;
        // �����豸
        VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
        // �߼��豸
        VkDevice device = VK_NULL_HANDLE;
        // Vulkan Memory Allocator(����AMD��GPUOpen�Ŷ�:https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator)
        VmaAllocator vmaAllocator;
        // Surface
        VkSurfaceKHR surface = VK_NULL_HANDLE;
        // ͼ�ζ���
        VkQueue graphicsQueue = VK_NULL_HANDLE;
        // չʾ����
        VkQueue presentQueue = VK_NULL_HANDLE;
        // ������
        VkSwapchainKHR swapChain = VK_NULL_HANDLE;
        // �������ϵ�ͼ��(������������ʱ���Զ����������ٽ�������ʱ���Զ�����)
        vector<VkImage> swapChainImages;
        // ��������ͼ��ĸ�ʽ
        VkFormat swapChainImageFormat;
        // ��������ͼ��Ĵ�С
        VkExtent2D swapChainExtent;
        // �����
        VkCommandPool commandPool = VK_NULL_HANDLE;

        // ------------------------------------------��������Vulkan����--------------------------------------------

        void CreateVkInstance();
        void CreateDebugMessenger();
        void CreatePhysicalDevice();
        void CreateLogicalDevice();
        void CreateMemoryAllocator();
        void CreateSurface();
        void CreateSwapChain();

        // ----------------------------------------����Vulkan����ĸ�������----------------------------------------

        bool CheckValidationLayerSupport();
        vector<const char*> GetRequiredExtensions();
        // ���������õ�VkDebugUtilsMessenger
        VkResult CreateDebugUtilsMessengerEXT(const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);
        // ����VkDebugUtilsMessenger
        void DestroyDebugUtilsMessengerEXT(VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);
        // ��������豸�Ƿ��������
        bool IsPhysicalDeviceSuitable(VkPhysicalDevice device);
        // ��ȡ�����豸���д�����
        QueueFamilyIndices GetQueueFamilyIndices(VkPhysicalDevice device);
        // ��������豸�Ƿ�֧������Ҫ����չ
        bool CheckDeviceExtensionSupport(VkPhysicalDevice device);
        // ��ȡ�����豸��֧�ֵĽ�������Ϣ
        SwapChainSupportDetails GetSwapChainSupportDetails(VkPhysicalDevice device);
        // ��ȡӲ��֧�ֵ���󳬲�������(һ�����ؼ���������)
        VkSampleCountFlagBits GetMaxUsableSampleCount();
        // ѡ�񽻻���ͼ���ʽ��ɫ�ʿռ�
        VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const vector<VkSurfaceFormatKHR>& availableFormats);
        // ѡ�񽻻�����չʾģʽ
        VkPresentModeKHR ChooseSwapPresentMode(const vector<VkPresentModeKHR> availablePresentModes);
        // ѡ�񽻻���ͼ��ֱ���
        VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
    };
}