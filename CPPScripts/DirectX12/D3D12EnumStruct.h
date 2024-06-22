#pragma once
#include "../pubh.h"
#include <DirectX/d3dx12.h>
#include <wrl.h>
#include <comdef.h>
#include <dxgi1_5.h>
#include <dxcapi.h>
#include "../PublicStruct.h"

// ����D3D12����ؿ�
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "dxcompiler.lib")

using Microsoft::WRL::ComPtr;

inline void ThrowIfFailed(HRESULT hr)
{
    if (FAILED(hr))
    {
        _com_error err(hr);
        OutputDebugString(err.ErrorMessage());
        // ��ʵ����throw��ʱ������ǰ�ErrorMessage���صĴ�����Ϣ��������
        // ����ErrorMessage�ķ���ֵ��const TCHAR*����throw��ʱ��ֻ�ܴ���const char*
        // ���TCHAR��char��ת������Unicode�ַ����º��鷳����������ֻ�׳�һ���̶��Ĵ�����Ϣ
        throw std::exception("D3D12 interface call failed !");
    }
}

namespace ZXEngine
{
    const uint32_t DX_MAX_FRAMES_IN_FLIGHT = 1;

    typedef uint32_t ZX_D3D12_TEXTURE_USAGE_FLAGS;
    typedef enum ZX_D3D12_TEXTURE_USAGE {
        ZX_D3D12_TEXTURE_USAGE_NONE_BIT = 0x00000000,
        ZX_D3D12_TEXTURE_USAGE_SRV_BIT  = 0x00000001,
        ZX_D3D12_TEXTURE_USAGE_RTV_BIT  = 0x00000002,
        ZX_D3D12_TEXTURE_USAGE_DSV_BIT  = 0x00000004,
    } ZX_D3D12_TEXTURE_USAGE;

    enum ZX_D3D12_RT_ROOT_PARAMETER
	{
        ZX_D3D12_RT_ROOT_PARAMETER_TLAS,
        ZX_D3D12_RT_ROOT_PARAMETER_OUTPUT_IMAGE,
        ZX_D3D12_RT_ROOT_PARAMETER_INDEX_BUFFER,
        ZX_D3D12_RT_ROOT_PARAMETER_VERTEX_BUFFER,
        ZX_D3D12_RT_ROOT_PARAMETER_MATERIAL_DATA,
        ZX_D3D12_RT_ROOT_PARAMETER_TEXTURE_2D,
        ZX_D3D12_RT_ROOT_PARAMETER_TEXTURE_CUBE,
        ZX_D3D12_RT_ROOT_PARAMETER_CONSTANT_BUFFER,
        ZX_D3D12_RT_ROOT_PARAMETER_NUM
	};

    struct ZXD3D12DrawRecord
    {
        uint32_t VAO = 0;
        uint32_t pipelineID = 0;
        uint32_t materialDataID = 0;
        uint32_t instanceNum = 0;
        uint32_t instanceBuffer = UINT32_MAX;
    };

    struct ZXD3D12DrawCommand
    {
        CommandType commandType = CommandType::NotCare;
        // Allocator��CommandList��ʵ����1��1�ģ�һ��Allocator���Զ�Ӧ���CommandList
        // ������һ��Allocator����Ӧ������CommandList�У�ֻ����һ��CommandList����Recording״̬
        // ������΢�е�͵���ˣ���ֱ�Ӹ��1��1��������������С
        vector<ComPtr<ID3D12CommandAllocator>> allocators;
        vector<ComPtr<ID3D12GraphicsCommandList4>> commandLists;
        bool inUse = false;
    };

    struct ZXD3D12Fence
    {
        UINT64 currentFence = 0;
        ComPtr<ID3D12Fence> fence = nullptr;
    };

    struct ZXD3D12DescriptorHandle
    {
        // �����������������
        uint32_t descriptorIdx = UINT32_MAX;
        // ���������������������
        uint32_t descriptorHeapIdx = UINT32_MAX;
        D3D12_DESCRIPTOR_HEAP_TYPE heapType = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    };

    struct ZXD3D12Texture
    {
        ComPtr<ID3D12Resource> texture = nullptr;
        ZXD3D12DescriptorHandle handleSRV = {};
        ZXD3D12DescriptorHandle handleRTV = {};
        ZXD3D12DescriptorHandle handleDSV = {};
        ZX_D3D12_TEXTURE_USAGE_FLAGS usageFlags = ZX_D3D12_TEXTURE_USAGE_NONE_BIT;
        bool inUse = false;
    };

    struct ZXD3D12Buffer
    {
        ComPtr<ID3D12Resource> buffer = nullptr;
        uint32_t size = 0;
        void* cpuAddress = nullptr;
        D3D12_GPU_VIRTUAL_ADDRESS gpuAddress = 0;
    };

    struct ZXD3D12AccelerationStructure
    {
        bool isBuilt = false;
        ZXD3D12Buffer as;
    };

    struct ZXD3D12ASGroup
    {
        vector<ZXD3D12AccelerationStructure> asGroup;
        bool inUse = false;
    };

    struct ZXD3D12InstanceBuffer
    {
        ZXD3D12Buffer buffer;
        D3D12_VERTEX_BUFFER_VIEW view = {};
        bool inUse = false;
    };

    struct ZXD3D12VAO
    {
        UINT indexCount = 0; // ��������
        ZXD3D12Buffer indexBuffer;
        D3D12_INDEX_BUFFER_VIEW indexBufferView = {};

        UINT vertexCount = 0; // ��������
        ZXD3D12Buffer vertexBuffer;
        D3D12_VERTEX_BUFFER_VIEW vertexBufferView = {};

        ZXD3D12AccelerationStructure blas;
        bool inUse = false;
    };

    struct ZXD3D12RenderBuffer
    {
        vector<uint32_t> renderBuffers;
        bool inUse = false;
    };

    struct ZXD3D12FBO
    {
        uint32_t colorBufferIdx = UINT32_MAX;
        uint32_t depthBufferIdx = UINT32_MAX;
        uint32_t positionBufferIdx = UINT32_MAX; // GBuffer
        uint32_t normalBufferIdx   = UINT32_MAX; // GBuffer
        FrameBufferType bufferType = FrameBufferType::Normal;
        ClearInfo clearInfo = {};
        bool inUse = false;
    };

    struct ZXD3D12Pipeline
    {
        uint32_t textureNum = 0;
        ComPtr<ID3D12RootSignature> rootSignature = nullptr;
        ComPtr<ID3D12PipelineState> pipelineState = nullptr;
        bool inUse = false;
	};

    struct ZXD3D12MaterialTextureSet
    {
        vector<ZXD3D12DescriptorHandle> textureHandles;
    };

    struct ZXD3D12MaterialData
    {
        vector<ZXD3D12Buffer> constantBuffers;
        vector<ZXD3D12MaterialTextureSet> textureSets;
        bool inUse = false;
    };

    struct ZXD3D12ASInstanceData
    {
        uint32_t VAO = 0;
        uint32_t hitGroupIdx = 0;
        uint32_t rtMaterialDataID = 0;
        Matrix4 transform;
    };

    struct ZXD3D12SBT
    {
        vector<ZXD3D12Buffer> buffers;
        UINT64 bufferSize = 0;
        UINT64 rayGenEntrySize = 0;
        UINT64 rayGenSectionSize = 0;
        UINT64 missEntrySize = 0;
        UINT64 missSectionSize = 0;
        UINT64 hitGroupEntrySize = 0;
        UINT64 hitGroupSectionSize = 0;
	};

    struct ZXD3D12RTPipeline
    {
        uint32_t tlasIdx = 0;
        ZXD3D12SBT SBT;
        ComPtr<ID3D12StateObject> pipeline;
        ComPtr<ID3D12RootSignature> rootSignature;
        vector<ComPtr<ID3D12DescriptorHeap>> descriptorHeaps;
        vector<ZXD3D12Buffer> constantBuffers;
    };

    struct ZXD3D12RTMaterialData
    {
        vector<ZXD3D12Buffer> buffers;
        bool inUse = false;
    };

    struct ZXD3D12DXILLibraryDesc
    {
        D3D12_DXIL_LIBRARY_DESC desc = {};
        vector<D3D12_EXPORT_DESC> exportDescs;
    };

    struct ZXD3D12ShaderBlob
    {
        ComPtr<IDxcBlob> blob;
        vector<wstring> exportNames;
    };

    struct ZXD3D12HitGroupDesc
	{
        wstring hitGroupName;
        wstring closestHitShaderName;
        wstring anyHitShaderName;
        wstring intersectionShaderName;
		D3D12_HIT_GROUP_DESC desc = {};
	};
}