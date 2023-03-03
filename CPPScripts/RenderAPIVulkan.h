#pragma once
#include "RenderAPI.h"
#include "Vulkan/VulkanEnumStruct.h"

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

    class RenderAPIVulkan
    {
    /// <summary>
    /// ��׼RenderAPI�ӿ�
    /// </summary>
    public:
        RenderAPIVulkan();
        ~RenderAPIVulkan() {};

        virtual unsigned int LoadTexture(const char* path, int& width, int& height);
        virtual void DeleteTexture(unsigned int id);
        virtual ShaderReference* LoadAndCompileShader(const char* path);
        virtual FrameBufferObject* CreateFrameBufferObject(FrameBufferType type, unsigned int width = 0, unsigned int height = 0);

        virtual void DeleteMesh(unsigned int VAO);
        virtual void SetUpStaticMesh(unsigned int& VAO, unsigned int& VBO, unsigned int& EBO, vector<Vertex> vertices, vector<unsigned int> indices);


    /// <summary>
    /// ʵ�ֱ�׼RenderAPI�ӿڵĸ�������������
    /// </summary>
    private:
        vector<VulkanVAO*> VulkanVAOArray;
        vector<VulkanTexture*> VulkanTextureArray;

        unsigned int GetNextVAOIndex();
        VulkanVAO* GetVAOByIndex(unsigned int idx);
        unsigned int GetNextTextureIndex();
        VulkanTexture* GetTextureByIndex(unsigned int idx);


    /// <summary>
    /// ������ʱһ���Գ�ʼ���ĺ���Vulkan�������ر���
    /// </summary>
    private:
        // �Ƿ�����֤��
        bool validationLayersEnabled = false;
        // MSAA����������
        VkSampleCountFlagBits msaaSamplesCount = VK_SAMPLE_COUNT_1_BIT;
        // Ӳ��֧�ֶ��ٱ��ĸ������Բ���
        float maxSamplerAnisotropy = 1.0f;

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
        // ��ȡӲ���豸����
        void GetPhysicalDeviceProperties();
        // ѡ�񽻻���ͼ���ʽ��ɫ�ʿռ�
        VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const vector<VkSurfaceFormatKHR>& availableFormats);
        // ѡ�񽻻�����չʾģʽ
        VkPresentModeKHR ChooseSwapPresentMode(const vector<VkPresentModeKHR> availablePresentModes);
        // ѡ�񽻻���ͼ��ֱ���
        VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);


    /// <summary>
    /// Vulkan��Դ������ؽӿ�(��Щ�ӿ�Create��������Ҫ�ֶ�Destroy)
    /// </summary>
    private:
        vector<VkRenderPass> allVulkanRenderPass;

        VulkanBuffer CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VmaMemoryUsage memoryUsage);
        void DestroyBuffer(VulkanBuffer buffer);

        UniformBuffer CreateUniformBuffer(const vector<ShaderProperty>& properties);

        VulkanImage CreateImage(uint32_t width, uint32_t height, uint32_t mipLevels, VkSampleCountFlagBits numSamples, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VmaMemoryUsage memoryUsage);
        void DestroyImage(VulkanImage image);
        void GenerateMipMaps(VkImage image, VkFormat format, int32_t width, int32_t height, uint32_t mipLevels);

        VkImageView CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels);
        void DestroyImageView(VkImageView imageView);

        VkSampler CreateSampler(uint32_t mipLevels);

        void CreateAllRenderPass();
        VkRenderPass CreateRenderPass(RenderPassType type);
        VkRenderPass GetRenderPass(RenderPassType type);
        void DestroyRenderPass(VkRenderPass renderPass);

        VkPipeline CreatePipeline(const string& path, const ShaderInfo& shaderInfo, VkDescriptorSetLayout& descriptorSetLayout, VkPipelineLayout& pipelineLayout);
        
        VkDescriptorSetLayout CreateDescriptorSetLayout(const ShaderInfo& info);
        VkPipelineLayout CreatePipelineLayout(const VkDescriptorSetLayout& descriptorSetLayout);
        VkDescriptorPool CreateDescriptorPool(const ShaderInfo& info);
        vector<VkDescriptorSet> CreateDescriptorSets(VkDescriptorPool descriptorPool, const vector<VkDescriptorSetLayout>& descriptorSetLayouts);

        VkShaderModule CreateShaderModule(vector<char> code);
        ShaderModuleSet CreateShaderModules(const string& path, const ShaderInfo& info);
        void DestroyShaderModules(ShaderModuleSet shaderModules);


    /// <summary>
    /// ���������ӿ�
    /// </summary>
    public:
        bool windowResized = false;

    private:
        VkFence immediateExeFence;
        VkCommandBuffer immediateExeCmd;

        uint32_t GetMipMapLevels(int width, int height);
        void InitImmediateCommand();
        void ImmediatelyExecute(std::function<void(VkCommandBuffer cmd)>&& function);
        void TransitionImageLayout(VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels, VkImageAspectFlags aspectMask, VkPipelineStageFlags srcStage, VkAccessFlags srcAccessMask, VkPipelineStageFlags dstStage, VkAccessFlags dstAccessMask);

        VkPipelineVertexInputStateCreateInfo GetVertexInputInfo();
        VkPipelineInputAssemblyStateCreateInfo GetAssemblyInfo(VkPrimitiveTopology topology);
        VkPipelineDynamicStateCreateInfo GetDynamicStateInfo(vector<VkDynamicState> dynamicStates);
        VkPipelineRasterizationStateCreateInfo GetRasterizationInfo(VkCullModeFlagBits cullMode, VkFrontFace frontFace);
        VkPipelineMultisampleStateCreateInfo GetPipelineMultisampleInfo(VkSampleCountFlagBits rasterizationSamples);
    };
}