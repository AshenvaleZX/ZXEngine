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
        /// ��׼RenderAPI�ӿ�
        /// </summary>
    public:
        RenderAPIVulkan();
        ~RenderAPIVulkan() {};

        virtual void BeginFrame();
        virtual void EndFrame();

        // ��Ⱦ״̬
        virtual void OnWindowSizeChange(uint32_t width, uint32_t height);
        virtual void SetRenderState(RenderStateSetting* state);
        virtual void SetViewPort(unsigned int width, unsigned int height, unsigned int xOffset = 0, unsigned int yOffset = 0);
        virtual void WaitForRenderFinish();

        // FrameBuffer
        virtual void SwitchFrameBuffer(uint32_t id);
        virtual void ClearFrameBuffer();
        virtual FrameBufferObject* CreateFrameBufferObject(FrameBufferType type, unsigned int width = 0, unsigned int height = 0);
        virtual FrameBufferObject* CreateFrameBufferObject(FrameBufferType type, const ClearInfo& clearInfo, unsigned int width = 0, unsigned int height = 0);
        virtual void DeleteFrameBufferObject(FrameBufferObject* FBO);

        // ����
        virtual unsigned int LoadTexture(const char* path, int& width, int& height);
        virtual unsigned int LoadCubeMap(const vector<string>& faces);
        virtual unsigned int GenerateTextTexture(unsigned int width, unsigned int height, unsigned char* data);
        virtual void DeleteTexture(unsigned int id);

        // Shader
        virtual ShaderReference* LoadAndSetUpShader(const char* path, FrameBufferType type);
        virtual void DeleteShader(uint32_t id);

        // ����
        virtual uint32_t CreateMaterialData();
        virtual void SetUpMaterial(ShaderReference* shaderReference, MaterialData* materialData);
        virtual void UseMaterialData(uint32_t ID);
        virtual void DeleteMaterialData(uint32_t id);

        // Draw
        virtual uint32_t AllocateDrawCommand(CommandType commandType);
        virtual void Draw(uint32_t VAO);
        virtual void GenerateDrawCommand(uint32_t id);

        // Mesh
        virtual void DeleteMesh(unsigned int VAO);
        virtual void SetUpStaticMesh(unsigned int& VAO, const vector<Vertex>& vertices, const vector<uint32_t>& indices);
        virtual void SetUpDynamicMesh(unsigned int& VAO, unsigned int vertexSize, unsigned int indexSize);
        virtual void UpdateDynamicMesh(unsigned int VAO, const vector<Vertex>& vertices, const vector<uint32_t>& indices);
        virtual void GenerateParticleMesh(unsigned int& VAO);

        // Shader����
        virtual void UseShader(unsigned int ID);
        virtual void SetShaderScalar(Material* material, const string& name, int value, bool allBuffer = false);
        virtual void SetShaderScalar(Material* material, const string& name, bool value, bool allBuffer = false);
        virtual void SetShaderScalar(Material* material, const string& name, float value, bool allBuffer = false);
        virtual void SetShaderScalar(Material* material, const string& name, uint32_t value, bool allBuffer = false);
        virtual void SetShaderVector(Material* material, const string& name, const Vector2& value, bool allBuffer = false);
        virtual void SetShaderVector(Material* material, const string& name, const Vector2& value, uint32_t idx, bool allBuffer = false);
        virtual void SetShaderVector(Material* material, const string& name, const Vector3& value, bool allBuffer = false);
        virtual void SetShaderVector(Material* material, const string& name, const Vector3& value, uint32_t idx, bool allBuffer = false);
        virtual void SetShaderVector(Material* material, const string& name, const Vector4& value, bool allBuffer = false);
        virtual void SetShaderVector(Material* material, const string& name, const Vector4& value, uint32_t idx, bool allBuffer = false);
        virtual void SetShaderMatrix(Material* material, const string& name, const Matrix3& value, bool allBuffer = false);
        virtual void SetShaderMatrix(Material* material, const string& name, const Matrix3& value, uint32_t idx, bool allBuffer = false);
        virtual void SetShaderMatrix(Material* material, const string& name, const Matrix4& value, bool allBuffer = false);
        virtual void SetShaderMatrix(Material* material, const string& name, const Matrix4& value, uint32_t idx, bool allBuffer = false);
        virtual void SetShaderTexture(Material* material, const string& name, uint32_t ID, uint32_t idx, bool allBuffer = false, bool isBuffer = false);
        virtual void SetShaderCubeMap(Material* material, const string& name, uint32_t ID, uint32_t idx, bool allBuffer = false, bool isBuffer = false);


        /// <summary>
        /// ��׼RayTracing�ӿ�
        /// </summary>
    public:
        // Pipeline
        virtual uint32_t CreateRayTracingPipeline(const RayTracingShaderPathGroup& rtShaderPathGroup);
        virtual void SwitchRayTracingPipeline(uint32_t rtPipelineID);

        // Material
        virtual uint32_t CreateRayTracingMaterialData();
        virtual void SetUpRayTracingMaterialData(Material* material);
        virtual void DeleteRayTracingMaterialData(uint32_t id);

        // ���ݸ���
        virtual void PushRayTracingMaterialData(Material* material);
        virtual void PushAccelerationStructure(uint32_t VAO, uint32_t rtMaterialDataID, const Matrix4& transform);

        // Ray Trace
        virtual void RayTrace(uint32_t commandID, const RayTracingPipelineConstants& rtConstants);

        // Acceleration Structure
        virtual void BuildTopLevelAccelerationStructure(uint32_t commandID);
        virtual void BuildBottomLevelAccelerationStructure(uint32_t VAO, bool isCompact);


        /// <summary>
        /// ������ʱһ���Գ�ʼ���ĺ���Vulkan�������ر���
        /// </summary>
    private:
        // Ĭ�ϵ������FrameBufferɫ�ʿռ�
        const VkFormat defaultImageFormat = VK_FORMAT_R8G8B8A8_UNORM;
        // �Ƿ�����֤��
        bool validationLayersEnabled = false;
        // MSAA����������
        VkSampleCountFlagBits msaaSamplesCount = VK_SAMPLE_COUNT_1_BIT;
        // Ӳ��֧�ֶ��ٱ��ĸ������Բ���
        float maxSamplerAnisotropy = 1.0f;
        // ��ǰ��MAX_FRAMES_IN_FLIGHT�еĵڼ�֡
        uint32_t currentFrame = 0;
        // ��СUBO����ƫ����
        VkDeviceSize minUniformBufferOffsetAlignment = 8;

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
        // ���д�ID��¼
        QueueFamilyIndices queueFamilyIndices;
        // �ṩ����������ʾ�����Frame Buffer
        uint32_t presentFBOIdx = 0;

        // ������
        VkSwapchainKHR swapChain = VK_NULL_HANDLE;
        // ������Image(������������ʱ���Զ����������ٽ�������ʱ���Զ�����)
        vector<VkImage> swapChainImages;
        // ������ImageView���ֶ��������ֶ�����
        vector<VkImageView> swapChainImageViews;
        // ��������ͼ��ĸ�ʽ
        VkFormat swapChainImageFormat;
        // ��������ͼ��Ĵ�С
        VkExtent2D swapChainExtent;
        // ��ǰ��һ֡���õ�Image�ڽ������������
        uint32_t curPresentImageIdx = 0;
        // ������Image���õ��ź���
        vector<VkSemaphore> presentImageAvailableSemaphores;
        // һ֡���ƽ�����Fence
        vector<VkFence> inFlightFences;

        // �����
        VkCommandPool commandPool = VK_NULL_HANDLE;

        // ------------------------------------------��������Vulkan����--------------------------------------------

        void CreateVkInstance();
        void CreateDebugMessenger();
        void CreatePhysicalDevice();
        void CreateLogicalDevice();
        void CreateMemoryAllocator();
        void CreateCommandPool();
        void CreateSurface();
        void CreateSwapChain();
        void CreatePresentFrameBuffer();

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

        // ���´���������
        void RecreateSwapChain();
        // �������������Դ
        void CleanUpSwapChain();
        // ����PresentBuffer
        void DestroyPresentFrameBuffer();


        /// <summary>
        /// Vulkan��Դ������ؽӿ�(��Щ�ӿ�Create��������Ҫ�ֶ�Destroy)
        /// </summary>
    private:
        vector<VulkanVAO*> VulkanVAOArray;
        vector<VulkanFBO*> VulkanFBOArray;
        vector<VulkanAttachmentBuffer*> VulkanAttachmentBufferArray;
        vector<VulkanTexture*> VulkanTextureArray;
        vector<VulkanPipeline*> VulkanPipelineArray;
        vector<VulkanMaterialData*> VulkanMaterialDataArray;
        vector<VulkanDrawCommand*> VulkanDrawCommandArray;

        vector<VkRenderPass> allVulkanRenderPass;
        map<uint32_t, uint32_t> meshsToDelete;
        map<uint32_t, uint32_t> texturesToDelete;
        map<uint32_t, uint32_t> materialDatasToDelete;
        map<uint32_t, uint32_t> pipelinesToDelete;

        uint32_t GetNextVAOIndex();
        VulkanVAO* GetVAOByIndex(uint32_t idx);
        void DestroyVAOByIndex(uint32_t idx);
        uint32_t GetNextFBOIndex();
        VulkanFBO* GetFBOByIndex(uint32_t idx);
        void DestroyFBOByIndex(uint32_t idx);
        uint32_t GetNextAttachmentBufferIndex();
        VulkanAttachmentBuffer* GetAttachmentBufferByIndex(uint32_t idx);
        void DestroyAttachmentBufferByIndex(uint32_t idx);
        uint32_t GetNextTextureIndex();
        VulkanTexture* GetTextureByIndex(uint32_t idx);
        void DestroyTextureByIndex(uint32_t idx);
        uint32_t GetNextPipelineIndex();
        VulkanPipeline* GetPipelineByIndex(uint32_t idx);
        void DestroyPipelineByIndex(uint32_t idx);
        uint32_t GetNextMaterialDataIndex();
        VulkanMaterialData* GetMaterialDataByIndex(uint32_t idx);
        void DestroyMaterialDataByIndex(uint32_t idx);
        uint32_t GetNextDrawCommandIndex();
        VulkanDrawCommand* GetDrawCommandByIndex(uint32_t idx);

        void* GetShaderPropertyAddress(ShaderReference* reference, uint32_t materialDataID, const string& name, uint32_t idx = 0);
        vector<void*> GetShaderPropertyAddressAllBuffer(ShaderReference* reference, uint32_t materialDataID, const string& name, uint32_t idx = 0);

        VulkanBuffer CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VmaMemoryUsage memoryUsage, bool cpuAddress = false, bool gpuAddress = false);
        void DestroyBuffer(VulkanBuffer buffer);

        UniformBuffer CreateUniformBuffer(const vector<ShaderProperty>& properties);
        void DestroyUniformBuffer(const UniformBuffer& uniformBuffer);

        void AllocateCommandBuffer(VkCommandBuffer& commandBuffers);
        void AllocateCommandBuffers(vector<VkCommandBuffer>& commandBuffers);

        void CreateVkFence(VkFence& fence);
        void CreateVkSemaphore(VkSemaphore& semaphore);

        VulkanImage CreateImage(uint32_t width, uint32_t height, uint32_t mipLevels, uint32_t layers, VkSampleCountFlagBits numSamples, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VmaMemoryUsage memoryUsage);
        void DestroyImage(VulkanImage image);
        void GenerateMipMaps(VkImage image, VkFormat format, int32_t width, int32_t height, uint32_t mipLevels);

        VkImageView CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, VkImageViewType viewType);
        void DestroyImageView(VkImageView imageView);

        VkSampler CreateSampler(uint32_t mipLevels);

        uint32_t CreateVulkanTexture(VulkanImage image, VkImageView imageView, VkSampler sampler);

        void CreateAllRenderPass();
        VkRenderPass CreateRenderPass(RenderPassType type);
        VkRenderPass GetRenderPass(RenderPassType type);
        void DestroyRenderPass(VkRenderPass renderPass);

        VkPipeline CreatePipeline(const string& path, const ShaderInfo& shaderInfo, VkDescriptorSetLayout& descriptorSetLayout, VkPipelineLayout& pipelineLayout, RenderPassType renderPassType);
        
        VkDescriptorSetLayout CreateDescriptorSetLayout(const ShaderInfo& info);
        VkDescriptorSetLayout CreateDescriptorSetLayout(const vector<VkDescriptorSetLayoutBinding>& bindings);
        VkPipelineLayout CreatePipelineLayout(const vector<VkDescriptorSetLayout>& descriptorSetLayouts, const vector<VkPushConstantRange>& pushConstantRanges);
        VkDescriptorPool CreateDescriptorPool(const ShaderInfo& info);
        vector<VkDescriptorSet> CreateDescriptorSets(VkDescriptorPool descriptorPool, const vector<VkDescriptorSetLayout>& descriptorSetLayouts);

        VkShaderModule CreateShaderModule(vector<char> code);
        ShaderModuleSet CreateShaderModules(const string& path, const ShaderInfo& info);
        void DestroyShaderModules(ShaderModuleSet shaderModules);

        // ����Ƿ�����Ҫ�ӳ�ж�ص���Դ
        void CheckDeleteData();


        /// <summary>
        /// Vulkan����׷�������Դ�ͽӿ�
        /// </summary>
    private:
        // GPUӲ����׷����
        VkPhysicalDeviceRayTracingPipelinePropertiesKHR rtPhysicalProperties;
        // GPUӲ��Acceleration Structure��Ϣ
        VkPhysicalDeviceAccelerationStructurePropertiesKHR physicalAccelerationStructureProperties;

        // ��ǰ�Ĺ���׷�ٹ���ID
        uint32_t curRTPipelineID = 0;
        // ����׷�ٹ���
        vector<VulkanRTPipeline*> rtPipelines;
        // Top Level Acceleration Structure
        vector<VulkanAccelerationStructure> allTLAS;
        // ����TLAS���м�Buffer
        vector<VulkanBuffer> rtTLASStagingBuffers;
        vector<VulkanBuffer> rtTLASScratchBuffers;
        vector<VulkanBuffer> rtTLASInstanceBuffers;

        // �����е���������
        uint32_t rtSceneTextureNum = 100;
        // �����е���Ⱦ��������
        uint32_t rtSceneRenderObjectNum = 100;

        // ��ǰ��һ֡Ҫ���ƵĶ�����Ϣ����
        vector<VulkanASInstanceData> asInstanceData;
        // ��ǰ����������������������
        vector<uint32_t> curRTSceneTextureIndexes;
        // ��ǰ������������������������������±��ӳ���
        unordered_map<uint32_t, uint32_t> curRTSceneTextureIndexMap;
        // ��ǰ���������й�׷������������
        vector<uint32_t> curRTSceneRTMaterialDatas;
        // ��ǰ���������й�׷���ʵ��������׷���������±��ӳ���
        unordered_map<uint32_t, uint32_t> curRTSceneRTMaterialDataMap;

        // ���еĹ�׷�������飬���п��ܰ��������ٵģ�δ�ڳ����е�
        vector<VulkanRTMaterialData*> VulkanRTMaterialDataArray;
        // ׼�����ĵĹ�׷����
        map<uint32_t, uint32_t> rtMaterialDatasToDelete;

        uint32_t GetNextRTMaterialDataIndex();
        VulkanRTMaterialData* GetRTMaterialDataByIndex(uint32_t idx);
        void DestroyRTMaterialDataByIndex(uint32_t idx);

        void CreateRTPipelineData(uint32_t id);
        void UpdateRTPipelineData(uint32_t id);

        void CreateRTSceneData(uint32_t id);
        void UpdateRTSceneData(uint32_t id);

        void* GetRTMaterialPropertyAddress(MaterialData* materialData, const string& name, uint32_t idx = 0);
        vector<void*> GetRTMaterialPropertyAddressAllBuffer(MaterialData* materialData, const string& name, uint32_t idx = 0);


        /// <summary>
        /// ���������ӿ�
        /// </summary>
    private:
        bool windowResized = false;
        uint32_t newWindowWidth = 0;
        uint32_t newWindowHeight = 0;

        uint32_t curFBOIdx = 0;
        uint32_t curPipeLineIdx = 0;
        uint32_t curMaterialDataIdx = 0;

        VkFence immediateExeFence;
        VkCommandBuffer immediateExeCmd;
        ViewPortInfo viewPortInfo;

        vector<VulkanDrawIndex> drawIndexes;
        vector<VkSemaphore> curWaitSemaphores;

        uint32_t GetCurFrameBufferIndex();
        uint32_t GetMipMapLevels(int width, int height);
        VkTransformMatrixKHR GetVkTransformMatrix(const Matrix4& mat);
        void InitImmediateCommand();
        void ImmediatelyExecute(std::function<void(VkCommandBuffer cmd)>&& function);
        void TransitionImageLayout(VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout, VkImageAspectFlags aspectMask, VkPipelineStageFlags srcStage, VkAccessFlags srcAccessMask, VkPipelineStageFlags dstStage, VkAccessFlags dstAccessMask);
        void TransitionImageLayout(VkCommandBuffer cmd, VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout, VkImageAspectFlags aspectMask, VkPipelineStageFlags srcStage, VkAccessFlags srcAccessMask, VkPipelineStageFlags dstStage, VkAccessFlags dstAccessMask);

        VkPipelineInputAssemblyStateCreateInfo GetAssemblyInfo(VkPrimitiveTopology topology);
        VkPipelineRasterizationStateCreateInfo GetRasterizationInfo(VkCullModeFlagBits cullMode);
        VkPipelineMultisampleStateCreateInfo GetPipelineMultisampleInfo(VkSampleCountFlagBits rasterizationSamples);
    };
}