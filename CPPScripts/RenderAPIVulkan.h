#pragma once
#include "RenderAPI.h"
#include "Vulkan/VulkanEnumStruct.h"

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

    class RenderAPIVulkan
    {
    /// <summary>
    /// 标准RenderAPI接口
    /// </summary>
    public:
        RenderAPIVulkan();
        ~RenderAPIVulkan() {};

        virtual void BeginFrame();
        virtual void EndFrame();

        // 渲染状态设置
        virtual void SetRenderState(RenderStateSetting* state);
        virtual void SetViewPort(unsigned int width, unsigned int height, unsigned int xOffset = 0, unsigned int yOffset = 0);

        // FrameBuffer相关
        virtual void SwitchFrameBuffer(unsigned int id);
        virtual void ClearFrameBuffer(const ClearInfo& clearInfo);
        virtual FrameBufferObject* CreateFrameBufferObject(FrameBufferType type, unsigned int width = 0, unsigned int height = 0);

        // 资源加载相关
        virtual unsigned int LoadTexture(const char* path, int& width, int& height);
        virtual void DeleteTexture(unsigned int id);
        virtual ShaderReference* LoadAndCompileShader(const char* path);
        virtual void SetUpMaterial(ShaderReference* shaderReference, const map<string, uint32_t>& textures);

        // Mesh设置
        virtual void DeleteMesh(unsigned int VAO);
        virtual void SetUpStaticMesh(unsigned int& VAO, unsigned int& VBO, unsigned int& EBO, vector<Vertex> vertices, vector<unsigned int> indices);
        
        // Shader设置
        virtual void SetShaderBool(ShaderReference* reference, const string& name, bool value);
        virtual void SetShaderInt(ShaderReference* reference, const string& name, int value);
        virtual void SetShaderFloat(ShaderReference* reference, const string& name, float value);
        virtual void SetShaderVector(ShaderReference* reference, const string& name, const Vector2& value);
        virtual void SetShaderVector(ShaderReference* reference, const string& name, float x, float y);
        virtual void SetShaderVector(ShaderReference* reference, const string& name, const Vector3& value);
        virtual void SetShaderVector(ShaderReference* reference, const string& name, float x, float y, float z);
        virtual void SetShaderVector(ShaderReference* reference, const string& name, const Vector4& value);
        virtual void SetShaderVector(ShaderReference* reference, const string& name, float x, float y, float z, float w);
        virtual void SetShaderMatrix(ShaderReference* reference, const string& name, const Matrix3& value);
        virtual void SetShaderMatrix(ShaderReference* reference, const string& name, const Matrix4& value);
        virtual void SetShaderTexture(ShaderReference* reference, const string& name, unsigned int textureID, unsigned int idx);


    /// <summary>
    /// 仅启动时一次性初始化的核心Vulkan组件及相关变量
    /// </summary>
    private:
        // 是否开启验证层
        bool validationLayersEnabled = false;
        // MSAA采样点数量
        VkSampleCountFlagBits msaaSamplesCount = VK_SAMPLE_COUNT_1_BIT;
        // 硬件支持多少倍的各项异性采样
        float maxSamplerAnisotropy = 1.0f;
        // 当前是MAX_FRAMES_IN_FLIGHT中的第几帧
        uint32_t currentFrame = 0;

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
        // 提供给交换链显示画面的Frame Buffer
        vector<VkFramebuffer> presentFrameBuffers;
        // 交换链
        VkSwapchainKHR swapChain = VK_NULL_HANDLE;
        // 交换链Image(创建交换链的时候自动创建，销毁交换链的时候自动销毁)
        vector<VkImage> swapChainImages;
        // 交换链ImageView，手动创建，手动销毁
        vector<VkImageView> swapChainImageViews;
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
        void CreatePresentFrameBuffer();

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
        // 获取硬件设备属性
        void GetPhysicalDeviceProperties();
        // 选择交换链图像格式和色彩空间
        VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const vector<VkSurfaceFormatKHR>& availableFormats);
        // 选择交换链的展示模式
        VkPresentModeKHR ChooseSwapPresentMode(const vector<VkPresentModeKHR> availablePresentModes);
        // 选择交换链图像分辨率
        VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);


    /// <summary>
    /// Vulkan资源创建相关接口(这些接口Create出来的需要手动Destroy)
    /// </summary>
    private:
        uint32_t curFBOIdx = 0;

        vector<VulkanVAO*> VulkanVAOArray;
        vector<VulkanFBO*> VulkanFBOArray;
        vector<VulkanTexture*> VulkanTextureArray;
        vector<VulkanPipeline*> VulkanPipelineArray;

        vector<VkRenderPass> allVulkanRenderPass;

        uint32_t GetNextVAOIndex();
        VulkanVAO* GetVAOByIndex(uint32_t idx);
        uint32_t GetNextFBOIndex();
        VulkanFBO* GetFBOByIndex(uint32_t idx);
        uint32_t GetNextTextureIndex();
        VulkanTexture* GetTextureByIndex(uint32_t idx);
        uint32_t GetNextPipelineIndex();
        VulkanPipeline* GetPipelineByIndex(uint32_t idx);
        void* GetShaderPropertyAddress(ShaderReference* reference, const string& name);

        VulkanBuffer CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VmaMemoryUsage memoryUsage);
        void DestroyBuffer(VulkanBuffer buffer);

        UniformBuffer CreateUniformBuffer(const vector<ShaderProperty>& properties);

        VulkanImage CreateImage(uint32_t width, uint32_t height, uint32_t mipLevels, VkSampleCountFlagBits numSamples, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VmaMemoryUsage memoryUsage);
        void DestroyImage(VulkanImage image);
        void GenerateMipMaps(VkImage image, VkFormat format, int32_t width, int32_t height, uint32_t mipLevels);

        VkImageView CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels);
        void DestroyImageView(VkImageView imageView);

        VkSampler CreateSampler(uint32_t mipLevels);

        uint32_t CreateVulkanTexture(VulkanImage image, VkImageView imageView, VkSampler sampler);

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
    /// 其它辅助接口
    /// </summary>
    public:
        bool windowResized = false;

    private:
        VkFence immediateExeFence;
        VkCommandBuffer immediateExeCmd;
        ViewPortInfo viewPortInfo;

        uint32_t GetMipMapLevels(int width, int height);
        void InitImmediateCommand();
        void ImmediatelyExecute(std::function<void(VkCommandBuffer cmd)>&& function);
        void TransitionImageLayout(VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels, VkImageAspectFlags aspectMask, VkPipelineStageFlags srcStage, VkAccessFlags srcAccessMask, VkPipelineStageFlags dstStage, VkAccessFlags dstAccessMask);

        VkPipelineVertexInputStateCreateInfo GetVertexInputInfo();
        VkPipelineInputAssemblyStateCreateInfo GetAssemblyInfo(VkPrimitiveTopology topology);
        VkPipelineDynamicStateCreateInfo GetDynamicStateInfo(vector<VkDynamicState> dynamicStates);
        VkPipelineRasterizationStateCreateInfo GetRasterizationInfo(VkCullModeFlagBits cullMode, VkFrontFace frontFace);
        VkPipelineMultisampleStateCreateInfo GetPipelineMultisampleInfo(VkSampleCountFlagBits rasterizationSamples);

        VkWriteDescriptorSet GetWriteDescriptorSet(VkDescriptorSet descriptorSet, const UniformBuffer& uniformBuffer);
        VkWriteDescriptorSet GetWriteDescriptorSet(VkDescriptorSet descriptorSet, VulkanTexture* texture, uint32_t binding);
    };
}