#pragma once
#include "RenderAPI.h"
#include "Vulkan/VulkanEnumStruct.h"

namespace ZXEngine
{
    class Material;
    class MaterialData;
    class RenderAPIVulkan : public RenderAPI
    {
        friend class EditorGUIManagerVulkan;
        friend class ImGuiTextureManagerVulkan;
        /// <summary>
        /// 标准渲染管线接口
        /// Standard Rendering Pipeline Interface
        /// </summary>
    public:
        RenderAPIVulkan();
        ~RenderAPIVulkan() {};

        virtual void BeginFrame();
        virtual void EndFrame();

        // 渲染状态
        virtual void OnWindowSizeChange(uint32_t width, uint32_t height);
        virtual void OnGameViewSizeChange();
        virtual void SetRenderState(RenderStateSetting* state);
        virtual void SetViewPort(unsigned int width, unsigned int height, unsigned int xOffset = 0, unsigned int yOffset = 0);
        virtual void WaitForRenderFinish();

        // Frame Buffer
        virtual void SwitchFrameBuffer(uint32_t id, uint32_t index = UINT32_MAX);
        virtual void ClearFrameBuffer(FrameBufferClearFlags clearFlags);
        virtual void BlitFrameBuffer(uint32_t cmd, const string& src, const string& dst, FrameBufferPieceFlags flags);
        virtual FrameBufferObject* CreateFrameBufferObject(FrameBufferType type, unsigned int width = 0, unsigned int height = 0);
        virtual FrameBufferObject* CreateFrameBufferObject(FrameBufferType type, const ClearInfo& clearInfo, unsigned int width = 0, unsigned int height = 0);
        virtual void DeleteFrameBufferObject(FrameBufferObject* FBO);
        virtual uint32_t GetRenderBufferTexture(uint32_t id);

        // Instance Buffer
        virtual uint32_t CreateStaticInstanceBuffer(uint32_t size, uint32_t num, const void* data);
        virtual uint32_t CreateDynamicInstanceBuffer(uint32_t size, uint32_t num);
        virtual void UpdateDynamicInstanceBuffer(uint32_t id, uint32_t size, uint32_t num, const void* data);
        virtual void SetUpInstanceBufferAttribute(uint32_t VAO, uint32_t instanceBuffer, uint32_t size, uint32_t offset = 6);
        virtual void DeleteInstanceBuffer(uint32_t id);

        // 纹理
        virtual unsigned int LoadTexture(const char* path, int& width, int& height);
        virtual unsigned int LoadCubeMap(const vector<string>& faces);
        virtual unsigned int CreateTexture(TextureFullData* data);
        virtual unsigned int CreateCubeMap(CubeMapFullData* data);
        virtual unsigned int GenerateTextTexture(unsigned int width, unsigned int height, unsigned char* data);
        virtual void DeleteTexture(unsigned int id);

        // Shader
        virtual ShaderReference* LoadAndSetUpShader(const string& path, FrameBufferType type);
        virtual ShaderReference* SetUpShader(const string& path, const string& shaderCode, FrameBufferType type);
        virtual void DeleteShader(uint32_t id);

        // 材质
        virtual uint32_t CreateMaterialData();
        virtual void SetUpMaterial(Material* material);
        virtual void UseMaterialData(uint32_t ID);
        virtual void DeleteMaterialData(uint32_t id);

        // Draw
        virtual uint32_t AllocateDrawCommand(CommandType commandType, FrameBufferClearFlags clearFlags);
        virtual void FreeDrawCommand(uint32_t commandID);
        virtual void Draw(uint32_t VAO);
        virtual void DrawInstanced(uint32_t VAO, uint32_t instanceNum, uint32_t instanceBuffer);
        virtual void GenerateDrawCommand(uint32_t id);

        // Mesh
        virtual void DeleteMesh(unsigned int VAO);
        virtual void SetUpStaticMesh(unsigned int& VAO, const vector<Vertex>& vertices, const vector<uint32_t>& indices, bool skinned = false);
        virtual void SetUpDynamicMesh(unsigned int& VAO, unsigned int vertexSize, unsigned int indexSize);
        virtual void UpdateDynamicMesh(unsigned int VAO, const vector<Vertex>& vertices, const vector<uint32_t>& indices);
        virtual void GenerateParticleMesh(unsigned int& VAO);

        // Shader参数
        virtual void UseShader(unsigned int ID);
        virtual void SetShaderScalar(Material* material, const string& name, bool value, bool allBuffer = false);
        virtual void SetShaderScalar(Material* material, const string& name, float value, bool allBuffer = false);
        virtual void SetShaderScalar(Material* material, const string& name, int32_t value, bool allBuffer = false);
        virtual void SetShaderScalar(Material* material, const string& name, uint32_t value, bool allBuffer = false);
        virtual void SetShaderVector(Material* material, const string& name, const Vector2& value, bool allBuffer = false);
        virtual void SetShaderVector(Material* material, const string& name, const Vector2& value, uint32_t idx, bool allBuffer = false);
        virtual void SetShaderVector(Material* material, const string& name, const Vector3& value, bool allBuffer = false);
        virtual void SetShaderVector(Material* material, const string& name, const Vector3& value, uint32_t idx, bool allBuffer = false);
        virtual void SetShaderVector(Material* material, const string& name, const Vector4& value, bool allBuffer = false);
        virtual void SetShaderVector(Material* material, const string& name, const Vector4& value, uint32_t idx, bool allBuffer = false);
        virtual void SetShaderVector(Material* material, const string& name, const Vector4* value, uint32_t count, bool allBuffer = false);
        virtual void SetShaderMatrix(Material* material, const string& name, const Matrix3& value, bool allBuffer = false);
        virtual void SetShaderMatrix(Material* material, const string& name, const Matrix3& value, uint32_t idx, bool allBuffer = false);
        virtual void SetShaderMatrix(Material* material, const string& name, const Matrix4& value, bool allBuffer = false);
        virtual void SetShaderMatrix(Material* material, const string& name, const Matrix4& value, uint32_t idx, bool allBuffer = false);
        virtual void SetShaderMatrix(Material* material, const string& name, const Matrix4* value, uint32_t count, bool allBuffer = false);
        virtual void SetShaderTexture(Material* material, const string& name, uint32_t ID, uint32_t idx, bool allBuffer = false, bool isBuffer = false);
        virtual void SetShaderCubeMap(Material* material, const string& name, uint32_t ID, uint32_t idx, bool allBuffer = false, bool isBuffer = false);


        /// <summary>
        /// 通用计算管线接口
        /// Compute Pipeline Interface
        /// </summary>
    public:
        // Shader Storage Buffer
        virtual uint32_t CreateShaderStorageBuffer(const void* data, size_t size, GPUBufferType type);
        virtual void BindShaderStorageBuffer(uint32_t id, uint32_t binding);
        virtual void UpdateShaderStorageBuffer(uint32_t id, const void* data, size_t size);
        virtual void DeleteShaderStorageBuffer(uint32_t id);

        // Vertex Buffer Binding
        virtual void BindVertexBuffer(uint32_t VAO, uint32_t binding);

        // Compute Shader
        virtual ComputeShaderReference* LoadAndSetUpComputeShader(const string& path);
        virtual void DeleteComputeShader(uint32_t id);

        // Compute Command
        virtual void Dispatch(uint32_t commandID, uint32_t shaderID, uint32_t groupX, uint32_t groupY, uint32_t groupZ, uint32_t xSizeInGroup, uint32_t ySizeInGroup, uint32_t zSizeInGroup);
        virtual void SubmitAllComputeCommands();


        /// <summary>
        /// 光线追踪管线接口
        /// Ray Tracing Pipeline Interface
        /// </summary>
    public:
        // Pipeline
        virtual uint32_t CreateRayTracingPipeline(const RayTracingShaderPathGroup& rtShaderPathGroup);
        virtual void SwitchRayTracingPipeline(uint32_t rtPipelineID);

        // Material
        virtual uint32_t CreateRayTracingMaterialData();
        virtual void SetUpRayTracingMaterialData(Material* material);
        virtual void DeleteRayTracingMaterialData(uint32_t id);

        // 数据更新
        virtual void SetRayTracingSkyBox(uint32_t textureID);
        virtual void PushRayTracingMaterialData(Material* material);
        virtual void PushAccelerationStructure(uint32_t VAO, uint32_t hitGroupIdx, uint32_t rtMaterialDataID, const Matrix4& transform);

        // Ray Trace
        virtual void RayTrace(uint32_t commandID, const RayTracingPipelineConstants& rtConstants);

        // Acceleration Structure
        virtual void BuildTopLevelAccelerationStructure(uint32_t commandID);
        virtual void BuildBottomLevelAccelerationStructure(uint32_t VAO, bool isCompact);


        /// <summary>
        /// 仅启动时一次性初始化的核心Vulkan组件及相关变量
        /// </summary>
    private:
        // 默认的纹理和FrameBuffer色彩空间
        const VkFormat defaultImageFormat = VK_FORMAT_R8G8B8A8_UNORM;
        // 是否开启验证层
        bool validationLayersEnabled = false;
        // MSAA采样点数量
        VkSampleCountFlagBits msaaSamplesCount = VK_SAMPLE_COUNT_1_BIT;
        // 硬件支持多少倍的各项异性采样
        float maxSamplerAnisotropy = 1.0f;
        // 当前是MAX_FRAMES_IN_FLIGHT中的第几帧
        uint32_t currentFrame = 0;
        // 最小UBO对齐偏移量
        VkDeviceSize minUniformBufferOffsetAlignment = 8;

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
        // 计算队列
        VkQueue computeQueue = VK_NULL_HANDLE;
        // 队列簇ID记录
        QueueFamilyIndices queueFamilyIndices;
        // 提供给交换链显示画面的Frame Buffer
        uint32_t presentFBOIdx = 0;

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
        // 当前这一帧所用的Image在交换链里的索引
        uint32_t curPresentImageIdx = 0;
        // 交换链Image可用的信号量
        vector<VkSemaphore> presentImageAvailableSemaphores;
        // 一帧绘制结束的Fence
        vector<VkFence> inFlightFences;
        // 计算管线Fence
        vector<VkFence> inFlightComputeFences;

        // 命令池
        VkCommandPool commandPool = VK_NULL_HANDLE;

        // ------------------------------------------建立各种Vulkan对象--------------------------------------------

        void CreateVkInstance();
        void CreateDebugMessenger();
        void CreatePhysicalDevice();
        void CreateLogicalDevice();
        void CreateMemoryAllocator();
        void CreateCommandPool();
        void CreateSurface();
        void CreateSwapChain();
        void CreatePresentFrameBuffer();

        // ----------------------------------------建立Vulkan对象的辅助函数----------------------------------------

        bool CheckValidationLayerSupport();
        vector<const char*> GetRequiredExtensions() const;
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

    public:
        // 重新创建交换链
        void RecreateSwapChain(bool recreateSurface = false);
    private:
        // 重新创建随GameView大小变化的Buffer
        void DoGameViewSizeChange();
        // 清理交换链相关资源
        void CleanUpSwapChain();
        // 销毁PresentBuffer
        void DestroyPresentFrameBuffer();


        /// <summary>
        /// Vulkan资源创建相关接口(这些接口Create出来的需要手动Destroy)
        /// </summary>
    private:
        vector<VulkanVAO*> VulkanVAOArray;
        vector<VulkanFBO*> VulkanFBOArray;
        vector<VulkanSSBO*> VulkanSSBOArray;
        vector<VulkanAttachmentBuffer*> VulkanAttachmentBufferArray;
        vector<VulkanTexture*> VulkanTextureArray;
        vector<VulkanPipeline*> VulkanPipelineArray;
        vector<VulkanComputePipeline*> VulkanComputePipelineArray;
        vector<VulkanMaterialData*> VulkanMaterialDataArray;
        vector<VulkanBuffer*> VulkanInstanceBufferArray;
        vector<VulkanDrawCommand*> VulkanDrawCommandArray;

        vector<VkRenderPass> allVulkanRenderPass;
        unordered_map<uint32_t, uint32_t> meshsToDelete;
        unordered_map<uint32_t, uint32_t> ssbosToDelete;
        unordered_map<uint32_t, uint32_t> texturesToDelete;
        unordered_map<uint32_t, uint32_t> materialDatasToDelete;
        unordered_map<uint32_t, uint32_t> pipelinesToDelete;
        unordered_map<uint32_t, uint32_t> computePipelinesToDelete;
        unordered_map<uint32_t, uint32_t> instanceBuffersToDelete;
        unordered_map<uint32_t, uint32_t> drawCommandsToDelete;

        uint32_t GetNextVAOIndex();
        VulkanVAO* GetVAOByIndex(uint32_t idx);
        void DestroyVAOByIndex(uint32_t idx);

        uint32_t GetNextFBOIndex();
        VulkanFBO* GetFBOByIndex(uint32_t idx);
        void DestroyFBOByIndex(uint32_t idx);

        uint32_t GetNextSSBOIndex();
        VulkanSSBO* GetSSBOByIndex(uint32_t idx);
        void DestroySSBOByIndex(uint32_t idx);

        uint32_t GetNextAttachmentBufferIndex();
        VulkanAttachmentBuffer* GetAttachmentBufferByIndex(uint32_t idx);
        void DestroyAttachmentBufferByIndex(uint32_t idx);

        uint32_t GetNextTextureIndex();
        VulkanTexture* GetTextureByIndex(uint32_t idx);
        void DestroyTextureByIndex(uint32_t idx);

        uint32_t GetNextPipelineIndex();
        VulkanPipeline* GetPipelineByIndex(uint32_t idx);
        void DestroyPipelineByIndex(uint32_t idx);

        uint32_t GetNextComputePipelineIndex();
        VulkanComputePipeline* GetComputePipelineByIndex(uint32_t idx);
        void DestroyComputePipelineByIndex(uint32_t idx);

        uint32_t GetNextMaterialDataIndex();
        VulkanMaterialData* GetMaterialDataByIndex(uint32_t idx);
        void DestroyMaterialDataByIndex(uint32_t idx);

        uint32_t GetNextInstanceBufferIndex();
        VulkanBuffer* GetInstanceBufferByIndex(uint32_t idx);
        void DestroyInstanceBufferByIndex(uint32_t idx);

        uint32_t GetNextDrawCommandIndex();
        VulkanDrawCommand* GetDrawCommandByIndex(uint32_t idx);
        void DestroyDrawCommandByIndex(uint32_t idx);

        void* GetShaderPropertyAddress(ShaderReference* reference, uint32_t materialDataID, const string& name, uint32_t idx = 0);
        vector<void*> GetShaderPropertyAddressAllBuffer(ShaderReference* reference, uint32_t materialDataID, const string& name, uint32_t idx = 0);

        VulkanBuffer CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VmaMemoryUsage memoryUsage, bool cpuAddress = false, bool gpuAddress = false, const void* data = nullptr);
        void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VmaMemoryUsage memoryUsage, VulkanBuffer& newBuffer, bool cpuAddress = false, bool gpuAddress = false, const void* data = nullptr);
        VulkanBuffer CreateGPUBuffer(VkDeviceSize size, VkBufferUsageFlags usage, const void* data);
        void CreateGPUBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkBuffer& buffer, VmaAllocation& allocation, const void* data);
        void DestroyBuffer(VulkanBuffer& buffer);

        UniformBuffer CreateUniformBuffer(const vector<ShaderProperty>& properties);
        void DestroyUniformBuffer(UniformBuffer& uniformBuffer);

        void AllocateCommandBuffer(VkCommandBuffer& commandBuffers);
        void AllocateCommandBuffers(vector<VkCommandBuffer>& commandBuffers);

        VulkanDescriptorGroup CreateDescriptorGroup(const VulkanComputePipeline* pipeline);
        void DestroyDescriptorGroup(VulkanDescriptorGroup& descriptorGroup);

        void CreateVkFence(VkFence& fence);
        void CreateVkSemaphore(VkSemaphore& semaphore);

        vector<VulkanImage*> VulkanImageArray;
        uint32_t CreateImage(uint32_t width, uint32_t height, uint32_t mipLevels, uint32_t layers, VkSampleCountFlagBits numSamples, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VmaMemoryUsage memoryUsage);
        VulkanImage* GetImage(uint32_t idx);
        void DestroyImage(uint32_t idx);

        void GenerateMipMaps(VkImage image, VkFormat format, int32_t width, int32_t height, uint32_t mipLevels);

        VkImageView CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, VkImageViewType viewType);
        // 为CubeMap的其中一个面创建独立的ImageView
        VkImageView CreateCubeFaceImageView(VkImage image, uint32_t face, VkFormat format, VkImageAspectFlags aspectFlags, VkImageViewType viewType);
        void DestroyImageView(VkImageView imageView);

        VkSampler CreateSampler(uint32_t mipLevels);

        uint32_t CreateVulkanTexture(uint32_t image, VkImageView imageView, VkSampler sampler);
        uint32_t CreateVulkanTexture(uint32_t image, const vector<VkImageView>& imageViews, VkSampler sampler);

        void CreateAllRenderPass();
        VkRenderPass CreateRenderPass(RenderPassType type);
        VkRenderPass GetRenderPass(RenderPassType type);
        void DestroyRenderPass(VkRenderPass renderPass);

        VkPipeline CreatePipeline(const string& path, const ShaderInfo& shaderInfo, VkDescriptorSetLayout& descriptorSetLayout, VkPipelineLayout& pipelineLayout, RenderPassType renderPassType);
        void DestroyVulkanPipeline(VulkanPipeline* pipeline);
        
        VkDescriptorSetLayout CreateDescriptorSetLayout(const ShaderInfo& info);
        VkDescriptorSetLayout CreateDescriptorSetLayout(const vector<VkDescriptorSetLayoutBinding>& bindings);
        VkPipelineLayout CreatePipelineLayout(const vector<VkDescriptorSetLayout>& descriptorSetLayouts, const vector<VkPushConstantRange>& pushConstantRanges);
        VkDescriptorPool CreateDescriptorPool(const ShaderInfo& info);
        vector<VkDescriptorSet> CreateDescriptorSets(VkDescriptorPool descriptorPool, const vector<VkDescriptorSetLayout>& descriptorSetLayouts);

        VkShaderModule CreateShaderModule(const vector<unsigned char>& code);
        ShaderModuleSet CreateShaderModules(const string& path, const ShaderInfo& info);
        void DestroyShaderModules(ShaderModuleSet shaderModules);

        // 检查是否有需要延迟卸载的资源
        void CheckDeleteData();


        /// <summary>
        /// Vulkan光线追踪相关资源和接口
        /// </summary>
    private:
        // GPU硬件光追属性
        VkPhysicalDeviceRayTracingPipelinePropertiesKHR rtPhysicalProperties;
        // GPU硬件Acceleration Structure信息
        VkPhysicalDeviceAccelerationStructurePropertiesKHR physicalAccelerationStructureProperties;

        // 当前使用的TLASGroup索引
        uint32_t curTLASGroupIdx = 0;
        // 当前的光线追踪管线ID
        uint32_t curRTPipelineID = 0;
        // 光线追踪管线
        vector<VulkanRTPipeline*> rtPipelines;
        // 构建TLAS的中间Buffer
        vector<VulkanBuffer> rtTLASStagingBuffers;
        vector<VulkanBuffer> rtTLASScratchBuffers;
        vector<VulkanBuffer> rtTLASInstanceBuffers;

        // 存放PushConstant数据的临时Buffer
        void* rtPushConstantBuffer = nullptr;
        // 存放PushConstant数据的临时Buffer的大小(64个32位数据)
        uint32_t rtPushConstantBufferSize = 256;

        // 在累积式光追场景中，用来判断画面刷新的数据
        vector<Matrix4> rtVPMatrix;
        vector<uint32_t> rtFrameCount;

        // 场景中的纹理数量
        uint32_t rtSceneTextureNum = 100;
        // 场景中的CubeMap数量
        uint32_t rtSceneCubeMapNum = 10;
        // 场景中的渲染对象数量
        uint32_t rtSceneRenderObjectNum = 100;

        // 当前这一帧要绘制的对象信息数组
        vector<VulkanASInstanceData> asInstanceData;
        // 当前场景中所有纹理索引数组
        vector<uint32_t> curRTSceneTextureIndexes;
        // 当前场景中所有纹理的索引与纹理数组下标的映射表
        unordered_map<uint32_t, uint32_t> curRTSceneTextureIndexMap;
        // 当前场景中所有CubeMap索引数组
        vector<uint32_t> curRTSceneCubeMapIndexes;
        // 当前场景中所有CubeMap的索引与CubeMap数组下标的映射表
        unordered_map<uint32_t, uint32_t> curRTSceneCubeMapIndexMap;
        // 当前场景中所有光追材质索引数组
        vector<uint32_t> curRTSceneRTMaterialDatas;
        // 当前场景中所有光追材质的索引与光追材质数组下标的映射表
        unordered_map<uint32_t, uint32_t> curRTSceneRTMaterialDataMap;

        // TLAS Group，一个场景有一个TLAS Group
        vector<VulkanASGroup*> VulkanTLASGroupArray;
        // 所有的光追材质数组，其中可能包括已销毁的，未在场景中的
        vector<VulkanRTMaterialData*> VulkanRTMaterialDataArray;
        // 准备消耗的光追材质
        map<uint32_t, uint32_t> rtMaterialDatasToDelete;

        uint32_t GetNextTLASGroupIndex();
        VulkanASGroup* GetTLASGroupByIndex(uint32_t idx);
        void DestroyTLASGroupByIndex(uint32_t idx);
        uint32_t GetNextRTMaterialDataIndex();
        VulkanRTMaterialData* GetRTMaterialDataByIndex(uint32_t idx);
        void DestroyRTMaterialDataByIndex(uint32_t idx);

        void DestroyAccelerationStructure(VulkanAccelerationStructure& accelerationStructure);

        void CreateRTPipelineData(uint32_t id);
        void UpdateRTPipelineData(uint32_t id);

        void CreateRTSceneData(uint32_t id);
        void UpdateRTSceneData(uint32_t id);

        void* GetRTMaterialPropertyAddress(MaterialData* materialData, const string& name, uint32_t idx = 0);
        vector<void*> GetRTMaterialPropertyAddressAllBuffer(MaterialData* materialData, const string& name, uint32_t idx = 0);


        /// <summary>
        /// 其它辅助接口
        /// </summary>
    private:
        bool gameViewResized = false;
        bool swapChainExpired = false;
        uint32_t newWindowWidth = 0;
        uint32_t newWindowHeight = 0;

        uint32_t curFBOIdx = 0;
		uint32_t curFBOInternalIdx = 0;
        uint32_t curPipeLineIdx = 0;
        uint32_t curMaterialDataIdx = 0;

        VkFence immediateExeFence;
        VkCommandBuffer immediateExeCmd;
        ViewPortInfo viewPortInfo;

        vector<VulkanDrawRecord> drawRecords;
        vector<VkSemaphore> curWaitSemaphores;

        vector<uint32_t> computeCommandRecords;
        vector<VkSemaphore> computeSemaphores;
        vector<bool> waitForComputeFenceOfLastFrame;

        using VulkanComputePipelineDescriptorBindingRecord = pair<uint32_t, uint32_t>;
        vector<VulkanComputePipelineDescriptorBindingRecord> curComputePipelineSSBOBindingRecords;
        vector<VulkanComputePipelineDescriptorBindingRecord> curComputePipelineVertexBufferBindingRecords;

        VkDescriptorSet GetNextDescriptorSet(VulkanComputePipeline* pipeline);

        uint32_t GetCurFrameBufferIndex() const;
        uint32_t GetMipMapLevels(int width, int height);
        VkTransformMatrixKHR GetVkTransformMatrix(const Matrix4& mat);
        void InitImmediateCommand();
        void ImmediatelyExecute(std::function<void(VkCommandBuffer cmd)>&& function);
        void TransitionImageLayout(VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout, VkImageAspectFlags aspectMask, VkPipelineStageFlags srcStage, VkAccessFlags srcAccessMask, VkPipelineStageFlags dstStage, VkAccessFlags dstAccessMask);
        void TransitionImageLayout(VkCommandBuffer cmd, VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout, VkImageAspectFlags aspectMask, VkPipelineStageFlags srcStage, VkAccessFlags srcAccessMask, VkPipelineStageFlags dstStage, VkAccessFlags dstAccessMask);

        VkPipelineInputAssemblyStateCreateInfo GetAssemblyInfo(VkPrimitiveTopology topology);
        VkPipelineRasterizationStateCreateInfo GetRasterizationInfo(VkCullModeFlagBits cullMode);
        VkPipelineMultisampleStateCreateInfo GetPipelineMultisampleInfo(VkSampleCountFlagBits rasterizationSamples);
        vector<unsigned char> GetSPIRVShader(const string& path, ShaderStageFlagBit stage);
    };
}