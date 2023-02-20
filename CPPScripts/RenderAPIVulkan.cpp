#include "RenderAPIVulkan.h"
#include "RenderEngine.h"

// VMA�Ĺٷ��ĵ���˵��Ҫ��һ��CPP�ļ��ﶨ������궨�壬������ܻ����쳣
// ��:https://gpuopen-librariesandsdks.github.io/VulkanMemoryAllocator/html/quick_start.html#quick_start_project_setup
#define VMA_IMPLEMENTATION
#include "vk_mem_alloc.h"

namespace ZXEngine
{
    // �Զ����Debug�ص�������VKAPI_ATTR��VKAPI_CALLȷ������ȷ�ĺ���ǩ�����Ӷ���Vulkan����
    static VKAPI_ATTR VkBool32 VKAPI_CALL VkDebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
    {
        std::cerr << "My debug call back: " << pCallbackData->pMessage << std::endl;
        // ����һ������ֵ����������validation layer��Ϣ��Vulkan�����Ƿ�Ӧ����ֹ
        // �������true������ý���VK_ERROR_VALIDATION_FAILED_EXT������ֹ
        // ��ͨ�����ڲ���validation layers���������������Ƿ���VK_FALSE
        return VK_FALSE;
    }

    RenderAPIVulkan::RenderAPIVulkan()
    {
        CreateVkInstance();
        CreateDebugMessenger();
        CreatePhysicalDevice();
        CreateLogicalDevice();
        CreateMemoryAllocator();
        CreateSurface();
        CreateSwapChain();

        InitImmediateCommand();
    }

    void RenderAPIVulkan::DeleteMesh(unsigned int VAO)
    {
        auto meshBuffer = GetVAOByIndex(VAO);
        meshBuffer->inUse = false;
        vmaDestroyBuffer(vmaAllocator, meshBuffer->indexBuffer, meshBuffer->indexBufferAlloc);
        vmaDestroyBuffer(vmaAllocator, meshBuffer->vertexBuffer, meshBuffer->vertexBufferAlloc);
    }

    void RenderAPIVulkan::SetUpStaticMesh(unsigned int& VAO, unsigned int& VBO, unsigned int& EBO, vector<Vertex> vertices, vector<unsigned int> indices)
    {
        VAO = GetNextVAOIndex();
        auto meshBuffer = GetVAOByIndex(VAO);

        // ----------------------------------------------- Vertex Buffer -----------------------------------------------
        VkDeviceSize vertexBufferSize = sizeof(Vertex) * vertices.size();

        // ����StagingBuffer
        VkBufferCreateInfo vertexStagingBufferInfo = {};
        vertexStagingBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        vertexStagingBufferInfo.size = vertexBufferSize;
        vertexStagingBufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        vertexStagingBufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        VmaAllocationCreateInfo vmaAllocInfo = {};
        vmaAllocInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_HOST;

        VkBuffer vertexStagingBuffer;
        VmaAllocation vertexStagingBufferAlloc;
        vmaCreateBuffer(vmaAllocator, &vertexStagingBufferInfo, &vmaAllocInfo, &vertexStagingBuffer, &vertexStagingBufferAlloc, nullptr);

        // �������ݵ�StagingBuffer
        void* vertexData;
        vmaMapMemory(vmaAllocator, vertexStagingBufferAlloc, &vertexData);
        memcpy(vertexData, vertices.data(), vertices.size() * sizeof(Vertex));
        vmaUnmapMemory(vmaAllocator, vertexStagingBufferAlloc);

        // ����VertexBuffer
        VkBufferCreateInfo vertexBufferInfo = {};
        vertexBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        vertexBufferInfo.size = vertexBufferSize;
        vertexBufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
        vertexBufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE; // ֻ��һ�����д�ʹ��

        vmaAllocInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;

        vmaCreateBuffer(vmaAllocator, &vertexBufferInfo, &vmaAllocInfo, &meshBuffer->vertexBuffer, &meshBuffer->vertexBufferAlloc, nullptr);

        // ��StagingBuffer������VertexBuffer
        ImmediatelyExecute([=](VkCommandBuffer cmd)
        {
            VkBufferCopy copy = {};
            copy.dstOffset = 0;
            copy.srcOffset = 0;
            copy.size = vertexBufferSize;
            vkCmdCopyBuffer(cmd, vertexStagingBuffer, meshBuffer->vertexBuffer, 1, &copy);
        });

        // ����StagingBuffer
        vmaDestroyBuffer(vmaAllocator, vertexStagingBuffer, vertexStagingBufferAlloc);

        // ----------------------------------------------- Index Buffer -----------------------------------------------
        VkDeviceSize indexBufferSize = sizeof(unsigned int) * indices.size();

        // ����StagingBuffer
        VkBufferCreateInfo indexStagingBufferInfo = {};
        indexStagingBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        indexStagingBufferInfo.size = indexBufferSize;
        indexStagingBufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        indexStagingBufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        vmaAllocInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_HOST;

        VkBuffer indexStagingBuffer;
        VmaAllocation indexStagingBufferAlloc;
        vmaCreateBuffer(vmaAllocator, &indexStagingBufferInfo, &vmaAllocInfo, &indexStagingBuffer, &indexStagingBufferAlloc, nullptr);

        // �������ݵ�StagingBuffer
        void* indexData;
        vmaMapMemory(vmaAllocator, indexStagingBufferAlloc, &indexData);
        memcpy(indexData, indices.data(), indices.size() * sizeof(unsigned int));
        vmaUnmapMemory(vmaAllocator, indexStagingBufferAlloc);

        // ����IndexBuffer
        VkBufferCreateInfo indexBufferInfo = {};
        indexBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        indexBufferInfo.size = indexBufferSize;
        indexBufferInfo.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
        indexBufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE; // ֻ��һ�����д�ʹ��

        vmaAllocInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;

        vmaCreateBuffer(vmaAllocator, &indexBufferInfo, &vmaAllocInfo, &meshBuffer->indexBuffer, &meshBuffer->indexBufferAlloc, nullptr);

        // ��StagingBuffer������IndexBuffer
        ImmediatelyExecute([=](VkCommandBuffer cmd)
        {
            VkBufferCopy copy = {};
            copy.dstOffset = 0;
            copy.srcOffset = 0;
            copy.size = indexBufferSize;
            vkCmdCopyBuffer(cmd, indexStagingBuffer, meshBuffer->indexBuffer, 1, &copy);
        });

        // ����StagingBuffer
        vmaDestroyBuffer(vmaAllocator, indexStagingBuffer, indexStagingBufferAlloc);

        meshBuffer->inUse = true;
    }

    unsigned int RenderAPIVulkan::GetNextVAOIndex()
    {
        unsigned int length = (unsigned int)VulkanVAOArray.size();
        
        for (unsigned int i = 0; i < length; i++)
        {
            if (!VulkanVAOArray[i]->inUse)
                return i;
        }

        VulkanVAOArray.push_back(new VulkanVAO());

        return length;
    }

    VulkanVAO* RenderAPIVulkan::GetVAOByIndex(unsigned int idx)
    {
        return VulkanVAOArray[idx];
    }


    // ------------------------------------------��������Vulkan����--------------------------------------------


    void RenderAPIVulkan::CreateVkInstance()
    {
        validationLayersEnabled = CheckValidationLayerSupport();

        // ������ݿ��Բ������������ݿ����ÿ���������Ӳ�����̣�����Nvidiaʲô��ʶ��һ�£���һЩ���������Ϸ�ߺ��������⴦��ʲô��
        VkApplicationInfo appInfo = {};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "ZXEngineApplication";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "ZXEngine";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_0;

        VkInstanceCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;

        // ��ȡ�벻ͬƽ̨�Ĵ���ϵͳ���н�������չ��Ϣ
        auto extensions = GetRequiredExtensions();
        createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
        createInfo.ppEnabledExtensionNames = extensions.data();

        // ���������֤������һ��
        if (validationLayersEnabled)
        {
            createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
            createInfo.ppEnabledLayerNames = validationLayers.data();
        }
        else
        {
            createInfo.enabledLayerCount = 0;
        }

        if (vkCreateInstance(&createInfo, nullptr, &vkInstance) != VK_SUCCESS)
            throw std::runtime_error("failed to create instance!");

        // ���������֤�㣬����һ�½���Debug��Ϣ�Ļص�
        if (validationLayersEnabled)
            CreateDebugMessenger();
    }

    void RenderAPIVulkan::CreateDebugMessenger()
    {
        VkDebugUtilsMessengerCreateInfoEXT createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        createInfo.pfnUserCallback = VkDebugCallback;

        if (CreateDebugUtilsMessengerEXT(&createInfo, nullptr, &debugMessenger) != VK_SUCCESS)
            throw std::runtime_error("failed to set up debug messenger!");
    }

    void RenderAPIVulkan::CreatePhysicalDevice()
    {
        // ��ȡ��ǰ�豸֧��Vulkan���Կ�����
        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(vkInstance, &deviceCount, nullptr);
        if (deviceCount == 0)
            throw std::runtime_error("failed to find GPUs with Vulkan support!");

        // ��ȡ����֧��Vulkan���Կ�����ŵ�devices��
        vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(vkInstance, &deviceCount, devices.data());

        // ���������Կ����ҵ���һ���������������
        // ��ʵ����д�����߳�ͬʱ������Щ�Կ������������������һ��
        for (const auto& device : devices)
        {
            if (IsPhysicalDeviceSuitable(device))
            {
                physicalDevice = device;
                msaaSamplesCount = GetMaxUsableSampleCount();
                break;
            }
        }

        if (physicalDevice == VK_NULL_HANDLE)
            throw std::runtime_error("failed to find a suitable GPU!");
    }

    void RenderAPIVulkan::CreateLogicalDevice()
    {
        // ��ȡ��ǰ�����豸�Ķ��д�����
        QueueFamilyIndices indices = GetQueueFamilyIndices(physicalDevice);

        // �߼��豸��Ҫ��ЩQueue
        vector<VkDeviceQueueCreateInfo> queueCreateInfos;
        float queuePriority = 1.0f;
        set<int> uniqueQueueFamilies = { indices.graphicsFamilyIdx, indices.presentFamilyIdx };
        // �ж�����дأ���������
        for (int queueFamily : uniqueQueueFamilies)
        {
            VkDeviceQueueCreateInfo queueCreateInfo = {};
            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            // ��ǰ���дض�Ӧ������
            queueCreateInfo.queueFamilyIndex = queueFamily;
            // ���д�����Ҫ����ʹ�õĶ�������
            queueCreateInfo.queueCount = 1;
            // Vulkan����ʹ��0.0��1.0֮��ĸ���������������ȼ���Ӱ���������ִ�еĵ��ã���ʹֻ��һ������Ҳ�Ǳ����
            queueCreateInfo.pQueuePriorities = &queuePriority;
            queueCreateInfos.push_back(queueCreateInfo);
        }

        // ��ȷ�豸Ҫʹ�õĹ�������
        VkPhysicalDeviceFeatures deviceFeatures = {};
        // ���öԸ������Բ�����֧��
        deviceFeatures.samplerAnisotropy = VK_TRUE;

        // �����߼��豸����Ϣ
        VkDeviceCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        // ʹ��ǰ��������ṹ�����
        createInfo.pQueueCreateInfos = queueCreateInfos.data();
        createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
        createInfo.pEnabledFeatures = &deviceFeatures;

        // ��VkInstanceһ����VkDevice���Կ�����չ����֤��
        // �����չ��Ϣ
        createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
        createInfo.ppEnabledExtensionNames = deviceExtensions.data();
        // �����������֤�㣬����֤����Ϣ��ӽ�ȥ
        if (validationLayersEnabled)
        {
            createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
            createInfo.ppEnabledLayerNames = validationLayers.data();
        }
        else
        {
            createInfo.enabledLayerCount = 0;
        }

        // ����vkCreateDevice����������ʵ�����߼��豸
        // �߼��豸����VkInstanceֱ�ӽ��������Բ�����ֻ�������豸
        if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS) {
            throw std::runtime_error("failed to create logical device!");
        }

        // �߼��豸������ʱ�򣬶���Ҳһ�𴴽��ˣ���ȡ���в�������������֮�����
        // �������߼��豸�����дأ����������ʹ洢��ȡ���б��������ָ��
        // ��Ϊ����ֻ�Ǵ�������дش���һ�����У�������Ҫʹ������0
        vkGetDeviceQueue(device, indices.graphicsFamilyIdx, 0, &graphicsQueue);
        vkGetDeviceQueue(device, indices.presentFamilyIdx, 0, &presentQueue);
    }

    void RenderAPIVulkan::CreateMemoryAllocator()
    {
        VmaAllocatorCreateInfo vmaInfo = {};
        vmaInfo.vulkanApiVersion = VK_HEADER_VERSION_COMPLETE;
        vmaInfo.instance = vkInstance;
        vmaInfo.physicalDevice = physicalDevice;
        vmaInfo.device = device;
        vmaCreateAllocator(&vmaInfo, &vmaAllocator);
    }

    void RenderAPIVulkan::CreateSurface() {
        // surface�ľ��崴��������Ҫ����ƽ̨�ģ�����ֱ����GLFW��װ�õĽӿ�������
        if (glfwCreateWindowSurface(vkInstance, RenderEngine::GetInstance()->window, nullptr, &surface) != VK_SUCCESS)
            throw std::runtime_error("failed to create window surface!");
    }

    void RenderAPIVulkan::CreateSwapChain()
    {
        // ��ѯӲ��֧�ֵĽ���������
        SwapChainSupportDetails swapChainSupport = GetSwapChainSupportDetails(physicalDevice);

        // ѡ��һ��ͼ���ʽ
        VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(swapChainSupport.formats);
        // ѡ��һ��presentģʽ(����ͼ�񽻻���ģʽ)
        VkPresentModeKHR presentMode = ChooseSwapPresentMode(swapChainSupport.presentModes);
        // ѡ��һ�����ʵ�ͼ��ֱ���
        VkExtent2D extent = ChooseSwapExtent(swapChainSupport.capabilities);

        // �������е�ͼ���������������Ϊ�������еĳ��ȡ���ָ������ʱͼ�����С���������ǽ����Դ���1��ͼ����������ʵ�����ػ��塣
        uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
        // ���maxImageCount����0���ͱ�ʾû�����ơ��������0�ͱ�ʾ�����ƣ���ô�������ֻ�����õ�maxImageCount
        if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
            imageCount = swapChainSupport.capabilities.maxImageCount;

        // �����������Ľṹ��
        VkSwapchainCreateInfoKHR createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        // �����ǵ�surface
        createInfo.surface = surface;
        // ��ǰ���ȡ����������
        createInfo.minImageCount = imageCount;
        createInfo.imageFormat = surfaceFormat.format;
        createInfo.imageColorSpace = surfaceFormat.colorSpace;
        createInfo.presentMode = presentMode;
        createInfo.imageExtent = extent;

        // imageArrayLayersָ��ÿ��ͼ���ж��ٲ㣬һ�㶼��1
        createInfo.imageArrayLayers = 1;
        // ����ֶ�ָ���ڽ������ж�ͼ����еľ������
        // �������ֱ�Ӷ����ǽ�����Ⱦ������ζ��������Ϊ��ɫ����
        // Ҳ�������Ƚ�ͼ����ȾΪ������ͼ�񣬽��к������
        // ����������¿���ʹ����VK_IMAGE_USAGE_TRANSFER_DST_BIT������ֵ����ʹ���ڴ��������Ⱦ��ͼ���䵽������ͼ�����
        createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        // ָ����δ���������дصĽ�����ͼ��
        QueueFamilyIndices indices = GetQueueFamilyIndices(physicalDevice);
        uint32_t queueFamilyIndices[] = { (uint32_t)indices.graphicsFamilyIdx, (uint32_t)indices.presentFamilyIdx };
        // ���graphics���д���presentation���дز�ͬ���������������
        // ���ǽ���graphics�����л��ƽ�������ͼ��Ȼ������һ��presentation�������ύ����
        // ����д���ͼ�������ַ���
        // VK_SHARING_MODE_EXCLUSIVE: ͬһʱ��ͼ��ֻ�ܱ�һ�����д�ռ�ã�����������д���Ҫ������Ȩ��Ҫ��ȷָ�������ַ�ʽ�ṩ����õ�����
        // VK_SHARING_MODE_CONCURRENT: ͼ����Ա�������дط��ʣ�����Ҫ��ȷ����Ȩ������ϵ
        // ������дز�ͬ����ʱʹ��concurrentģʽ�����⴦��ͼ������Ȩ������ϵ�����ݣ���Ϊ��Щ���漰���ٸ���
        if (indices.graphicsFamilyIdx != indices.presentFamilyIdx)
        {
            createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            // Concurrentģʽ��ҪԤ��ָ�����д�����Ȩ������ϵ��ͨ��queueFamilyIndexCount��pQueueFamilyIndices�������й���
            createInfo.queueFamilyIndexCount = 2;
            createInfo.pQueueFamilyIndices = queueFamilyIndices;
        }
        // ���graphics���дغ�presentation���д���ͬ��������Ҫʹ��exclusiveģʽ����Ϊconcurrentģʽ��Ҫ����������ͬ�Ķ��д�
        else
        {
            createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
            createInfo.queueFamilyIndexCount = 0; // Optional
            createInfo.pQueueFamilyIndices = nullptr; // Optional
        }

        // ���������֧��(supportedTransforms in capabilities)�����ǿ���Ϊ������ͼ��ָ��ĳЩת���߼�
        // ����90��˳ʱ����ת����ˮƽ��ת���������Ҫ�κ�transoform���������Լ򵥵�����ΪcurrentTransoform
        createInfo.preTransform = swapChainSupport.capabilities.currentTransform;

        // compositeAlpha�ֶ�ָ��alphaͨ���Ƿ�Ӧ�����������Ĵ���ϵͳ���л�ϲ�����������Ըù��ܣ��򵥵���VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR
        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

        // ���clipped��Ա����ΪVK_TRUE����ζ�����ǲ����ı��ڱε��������ݣ��������������Ĵ�������ǰ��ʱ������Ⱦ�Ĳ������ݴ����ڿ�������֮��
        // ���������Ҫ��ȡ��Щ���ػ����ݽ��д���������Կ����ü�����������
        createInfo.clipped = VK_TRUE;

        // Vulkan����ʱ��������������ĳЩ�����±��滻�����細�ڵ�����С���߽�������Ҫ���·�������ͼ�����
        // ����������£�������ʵ������Ҫ���·��䴴���������ڴ��ֶ���ָ���Ծɵ����ã����Ի�����Դ
        // Ҳ�����Լ��ֶ����پɽ����������´�������ʹ���������
        // ����������������һ�㣬��������ھɽ���������չʾ��ʱ���Ȼ��ƺ���ͼ���½�������������Ҫ�Ⱦɽ���������ִ���꣬�����٣������µģ��ٻ���
        createInfo.oldSwapchain = VK_NULL_HANDLE;

        // ����������
        if (vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapChain) != VK_SUCCESS) {
            throw std::runtime_error("failed to create swap chain!");
        }

        // ��ȡһ�½�������ͼ������������imageCount�����ʼ������������ͼ������������ʵ�ʴ����Ĳ�һ������ô�࣬����Ҫ���»�ȡһ��
        vkGetSwapchainImagesKHR(device, swapChain, &imageCount, nullptr);
        // ����ʵ��ͼ��������������vector��С
        swapChainImages.resize(imageCount);
        // �ѽ������ϵ�ͼ��洢��swapChainImages��
        vkGetSwapchainImagesKHR(device, swapChain, &imageCount, swapChainImages.data());
        // ��һ�½�����ͼ��ĸ�ʽ�ͷֱ���
        swapChainImageFormat = surfaceFormat.format;
        swapChainExtent = extent;
    }


    // ----------------------------------------����Vulkan����ĸ�������----------------------------------------


    bool RenderAPIVulkan::CheckValidationLayerSupport()
    {
        // ��ȡ���п��õ�Layer����
        uint32_t layerCount = 0;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

        // ��ȡ���п���Layer������
        vector<VkLayerProperties> availableLayers(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

        // ������ǵ�validationLayers�е�����layer�Ƿ������availableLayers�б���
        for (const char* layerName : validationLayers)
        {
            bool layerFound = false;
            for (const auto& layerProperties : availableLayers)
            {
                if (strcmp(layerName, layerProperties.layerName) == 0)
                {
                    layerFound = true;
                    break;
                }
            }
            if (!layerFound)
                return false;
        }

        return true;
    }

    vector<const char*> RenderAPIVulkan::GetRequiredExtensions()
    {
        // Vulakn����ƽ̨��������API֧�ֵ�(������ʱ����)������ζ����Ҫһ����չ�����벻ͬƽ̨�Ĵ���ϵͳ���н���
        // GLFW��һ����������ú������������йص���չ��Ϣ
        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions;
        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        vector<const char*> extensions;

        // ���GLFW��ȡ����չ
        for (unsigned int i = 0; i < glfwExtensionCount; i++)
            extensions.push_back(glfwExtensions[i]);

        // ���������Debug�����Debug����չ
        if (validationLayersEnabled)
            extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

        return extensions;
    }

    // Vulkan��Debug����չ���ܣ�����vkCreateDebugUtilsMessengerEXT�������Զ�����
    // ���������Ҫͨ��vkGetInstanceProcAddr�ֶ���ȡ������ַ
    // ����������Ƿ�װ��һ�»�ȡvkCreateDebugUtilsMessengerEXT��ַ���������Ĺ���
    VkResult RenderAPIVulkan::CreateDebugUtilsMessengerEXT(const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger)
    {
        auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(vkInstance, "vkCreateDebugUtilsMessengerEXT");
        if (func != nullptr)
            return func(vkInstance, pCreateInfo, pAllocator, pDebugMessenger);
        else
            return VK_ERROR_EXTENSION_NOT_PRESENT;
    }

    // ���������ĺ���������VkDebugUtilsMessengerEXT��������Ҫ�ֶ�����vkDestroyDebugUtilsMessengerEXT����
    void RenderAPIVulkan::DestroyDebugUtilsMessengerEXT(VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator)
    {
        auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(vkInstance, "vkDestroyDebugUtilsMessengerEXT");
        if (func != nullptr)
            func(vkInstance, debugMessenger, pAllocator);
    }

    bool RenderAPIVulkan::IsPhysicalDeviceSuitable(VkPhysicalDevice device)
    {
        // ���name, type�Լ�Vulkan�汾�Ȼ������豸����
        VkPhysicalDeviceProperties deviceProperties;
        vkGetPhysicalDeviceProperties(device, &deviceProperties);
        // ��Ҫ����
        if (deviceProperties.deviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
            return false;

        // ��ѯ������ѹ����64λ�������Ͷ���ͼ��Ⱦ(VR�ǳ�����)�ȿ�ѡ���ܵ�֧��
        VkPhysicalDeviceFeatures deviceFeatures;
        vkGetPhysicalDeviceFeatures(device, &deviceFeatures);
        // ��Ҫ֧�ּ�����ɫ��
        if (!deviceFeatures.geometryShader)
            return false;

        // ��Ҫ֧�ָ������Բ���
        if (!deviceFeatures.samplerAnisotropy)
            return false;

        // ��ѯ���������Ķ��д�
        QueueFamilyIndices indices = GetQueueFamilyIndices(device);
        if (!indices.isComplete())
            return false;

        // ����Ƿ�֧������Ҫ����չ
        if (!CheckDeviceExtensionSupport(device))
            return false;

        // ��齻�����Ƿ�����
        SwapChainSupportDetails swapChainSupport = GetSwapChainSupportDetails(device);
        // ����֧��һ��ͼ���ʽ��һ��Presentģʽ
        bool swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
        if (!swapChainAdequate)
            return false;

        // ����кܶ��Կ�������ͨ��������Ӳ������һ��Ȩ�أ�Ȼ������ѡ������ʵ�
        return true;
    }

    QueueFamilyIndices RenderAPIVulkan::GetQueueFamilyIndices(VkPhysicalDevice device)
    {
        QueueFamilyIndices indices;

        // ��ȡ�豸ӵ�еĶ��д�����
        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

        // ��ȡ���ж��дػ������
        vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

        // �������дأ���ȡ֧����������Ķ��д�
        int i = 0;
        for (const auto& queueFamily : queueFamilies)
        {
            // ��ǰ���д��Ƿ�֧��ͼ�δ���
            if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
                indices.graphicsFamilyIdx = i;

            // �Ƿ�֧��VkSurfaceKHR
            VkBool32 presentSupport = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);
            // ֧�ֵĻ���¼һ������
            if (queueFamily.queueCount > 0 && presentSupport)
                indices.presentFamilyIdx = i;

            // ע������֧��surface��graphic�Ķ��дز�һ����ͬһ��
            // ����ʹ����Щ���дص��߼���Ҫô�̶�����֧��surface��graphic�Ķ��д���������ͬ��������(���������ǲ���ͬһ�����������)
            // Ҫô������Ҷ��дص�ʱ����ȷָ������ͬʱ֧��surface��graphic��Ȼ�󱣴�ͬʱ֧��������Ҫ��Ķ��д�����(���ܿ��ܻ�õ�)

            if (indices.isComplete())
                break;

            i++;
        }

        // ���ؾ߱�ͼ�������Ķ���
        return indices;
    }

    bool RenderAPIVulkan::CheckDeviceExtensionSupport(VkPhysicalDevice device) {
        // ��ȡ�����豸֧�ֵ���չ����
        uint32_t extensionCount;
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

        // ��ȡ��֧�ֵľ�����Ϣ
        vector<VkExtensionProperties> availableExtensions(extensionCount);
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

        // �������Զ����deviceExtensionsת����set���ݽṹ(Ϊ�˱������2��forѭ����erase��ͬʱҲ���Ķ�ԭ����)
        set<string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());
        // ���������豸��֧�ֵ���չ�������������Ҫ����չ������ɾ��
        for (const auto& extension : availableExtensions)
            requiredExtensions.erase(extension.extensionName);

        // ���ȫ��ɾ�����ˣ�˵����������Ҫ����չ����֧�ֵģ�����˵������һЩ������Ҫ����չ����֧��
        return requiredExtensions.empty();
    }

    SwapChainSupportDetails RenderAPIVulkan::GetSwapChainSupportDetails(VkPhysicalDevice device)
    {
        SwapChainSupportDetails details;

        // ����VkPhysicalDevice��VkSurfaceKHR��ȡ��֧�ֵľ��幦�ܣ������������ǽ������ĺ������
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

        // ��ѯ֧�ֵ�surface��ʽ����Щ
        uint32_t formatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);
        if (formatCount != 0)
        {
            // ����ʽ����
            details.formats.resize(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
        }

        // ��ѯ֧�ֵ�surfaceģʽ����Щ
        uint32_t presentModeCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);
        if (presentModeCount != 0) {
            details.presentModes.resize(presentModeCount);
            // ���ģʽ����
            vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
        }

        return details;
    }

    VkSampleCountFlagBits RenderAPIVulkan::GetMaxUsableSampleCount()
    {
        VkPhysicalDeviceProperties physicalDeviceProperties;
        vkGetPhysicalDeviceProperties(physicalDevice, &physicalDeviceProperties);

        // ȡͬʱ֧��Color��Depth���������
        VkSampleCountFlags counts = physicalDeviceProperties.limits.framebufferColorSampleCounts & physicalDeviceProperties.limits.framebufferDepthSampleCounts;
        if (counts & VK_SAMPLE_COUNT_64_BIT) { return VK_SAMPLE_COUNT_64_BIT; }
        if (counts & VK_SAMPLE_COUNT_32_BIT) { return VK_SAMPLE_COUNT_32_BIT; }
        if (counts & VK_SAMPLE_COUNT_16_BIT) { return VK_SAMPLE_COUNT_16_BIT; }
        if (counts & VK_SAMPLE_COUNT_8_BIT) { return VK_SAMPLE_COUNT_8_BIT; }
        if (counts & VK_SAMPLE_COUNT_4_BIT) { return VK_SAMPLE_COUNT_4_BIT; }
        if (counts & VK_SAMPLE_COUNT_2_BIT) { return VK_SAMPLE_COUNT_2_BIT; }

        return VK_SAMPLE_COUNT_1_BIT;
    }

    VkSurfaceFormatKHR RenderAPIVulkan::ChooseSwapSurfaceFormat(const vector<VkSurfaceFormatKHR>& availableFormats)
    {
        // ��һ����û����������ĸ�ʽ��ɫ�ʿռ���ϣ����������
        for (const auto& availableFormat : availableFormats)
        {
            if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
                return availableFormat;
        }

        // ���û������ĸ�ʽ������ֱ��ѡ���һ����ʽ
        // ��ʵ���ʱ������Ҳ���Ա���һ��availableFormats���Լ�дһ��������һ����ԽϺõĳ���
        return availableFormats[0];
    }

    VkPresentModeKHR RenderAPIVulkan::ChooseSwapPresentMode(const vector<VkPresentModeKHR> availablePresentModes) {
        // ��Vulkan�����ĸ�ģʽ����ʹ��:
        // 1��VK_PRESENT_MODE_IMMEDIATE_KHR
        // Ӧ�ó����ύ��ͼ���������䵽��Ļ���֣�����ģʽ���ܻ����˺��Ч����
        // 2��VK_PRESENT_MODE_FIFO_KHR
        // ������������һ�����У�����ʾ������Ҫˢ�µ�ʱ����ʾ�豸�Ӷ��е�ǰ���ȡͼ�񣬲��ҳ�����Ⱦ��ɵ�ͼ�������еĺ��档������������ĳ����ȴ������ֹ�ģ����Ƶ��Ϸ�Ĵ�ֱͬ�������ơ���ʾ�豸��ˢ��ʱ�̱���Ϊ����ֱ�жϡ���
        // 3��VK_PRESENT_MODE_FIFO_RELAXED_KHR
        // ��ģʽ����һ��ģʽ���в�ͬ�ĵط�Ϊ�����Ӧ�ó�������ӳ٣����������һ����ֱͬ���ź�ʱ���п��ˣ�������ȴ���һ����ֱͬ���źţ����ǽ�ͼ��ֱ�Ӵ��͡����������ܵ��¿ɼ���˺��Ч����
        // 4��VK_PRESENT_MODE_MAILBOX_KHR
        // ���ǵڶ���ģʽ�ı��֡�����������������ʱ��ѡ���µ��滻�ɵ�ͼ�񣬴Ӷ��������Ӧ�ó�������Ρ�����ģʽͨ������ʵ�����ػ����������׼�Ĵ�ֱͬ��˫������ȣ���������Ч�����ӳٴ�����˺��Ч����

        // Ĭ�ϵ�ģʽ
        VkPresentModeKHR bestMode = VK_PRESENT_MODE_FIFO_KHR;

        for (const auto& availablePresentMode : availablePresentModes)
        {
            if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
                return availablePresentMode;
            else if (availablePresentMode == VK_PRESENT_MODE_IMMEDIATE_KHR)
                bestMode = availablePresentMode;
        }

        return bestMode;
    }

    VkExtent2D RenderAPIVulkan::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) {
        // ���capabilities������ͨ��vkGetPhysicalDeviceSurfaceCapabilitiesKHR�ӿڲ�ѯ������

        // currentExtent�ĸ߿������һ�������uint32���ֵ����˵�����������Լ���һ����Χ�������������ֵ
        if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
        {
            int width, height;
            glfwGetFramebufferSize(RenderEngine::GetInstance()->window, &width, &height);

            VkExtent2D actualExtent =
            {
                static_cast<uint32_t>(width),
                static_cast<uint32_t>(height)
            };

            // ������minImageExtent��maxImageExtent֮��
            actualExtent.width = Math::Max(capabilities.minImageExtent.width, Math::Min(capabilities.maxImageExtent.width, actualExtent.width));
            actualExtent.height = Math::Max(capabilities.minImageExtent.height, Math::Min(capabilities.maxImageExtent.height, actualExtent.height));

            return actualExtent;
        }
        // ��������ֻ��ʹ�ò�ѯ����currentExtent
        else
        {
            return capabilities.currentExtent;
        }
    }


    void RenderAPIVulkan::InitImmediateCommand()
    {
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = commandPool;
        allocInfo.commandBufferCount = 1;
        vkAllocateCommandBuffers(device, &allocInfo, &immediateExeCmd);

        VkFenceCreateInfo fenceInfo{};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        // ����ʱ��������Ϊsignaled״̬(�����һ����Զ�Ȳ���)
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
        vkCreateFence(device, &fenceInfo, nullptr, &immediateExeFence);
    }

    void RenderAPIVulkan::ImmediatelyExecute(std::function<void(VkCommandBuffer cmd)>&& function)
    {
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        vkBeginCommandBuffer(immediateExeCmd, &beginInfo);

        function(immediateExeCmd);

        vkEndCommandBuffer(immediateExeCmd);

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &immediateExeCmd;
        vkQueueSubmit(graphicsQueue, 1, &submitInfo, immediateExeFence);

        vkWaitForFences(device, 1, &immediateExeFence, VK_TRUE, UINT64_MAX);
        vkResetFences(device, 1, &immediateExeFence);
    }
}