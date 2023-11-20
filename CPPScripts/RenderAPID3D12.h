#pragma once
#include "RenderAPI.h"
#include "DirectX12/D3D12EnumStruct.h"

namespace ZXEngine
{
	class RenderAPID3D12 : public RenderAPI
	{
		friend class ImGuiTextureManagerD3D12;
		friend class EditorGUIManagerDirectX12;
		friend class ZXD3D12DescriptorAllocator;
		/// <summary>
		/// 标准RenderAPI接口
		/// </summary>
	public:
		RenderAPID3D12();
		~RenderAPID3D12() {};

		virtual void BeginFrame();
		virtual void EndFrame();

		// 渲染状态
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

		// 纹理
		virtual unsigned int LoadTexture(const char* path, int& width, int& height);
		virtual unsigned int LoadCubeMap(const vector<string>& faces);
		virtual unsigned int GenerateTextTexture(unsigned int width, unsigned int height, unsigned char* data);
		virtual void DeleteTexture(unsigned int id);

		// Shader
		virtual ShaderReference* LoadAndSetUpShader(const char* path, FrameBufferType type);
		virtual void DeleteShader(uint32_t id);

		// 材质
		virtual uint32_t CreateMaterialData();
		virtual void SetUpMaterial(Material* material);
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
		/// 标准RayTracing接口(暂未实现基于DXR的光线追踪)
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
		/// 仅启动时一次性初始化的核心D3D12组件及相关变量
		/// </summary>
	private:
		// 各类型描述符大小
		UINT mRtvDescriptorSize = 0;
		UINT mDsvDescriptorSize = 0;
		UINT mCbvSrvUavDescriptorSize = 0;
		// 渲染时动态分配的描述符堆
		vector<UINT> mDynamicDescriptorOffsets;
		vector<ComPtr<ID3D12DescriptorHeap>> mDynamicDescriptorHeaps;

		// 屏幕后台缓冲区图像格式
		DXGI_FORMAT mPresentBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
		// 默认的纹理和FrameBuffer色彩空间
		const DXGI_FORMAT mDefaultImageFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
		// 4X MSAA质量等级
		UINT m4xMSAAQuality = 0;
		UINT msaaSamplesCount = 4;

		// 当前是DX_MAX_FRAMES_IN_FLIGHT中的第几帧
		uint32_t mCurrentFrame = 0;
		// 当前这一帧要写入的Present Buffer下标
		uint32_t mCurPresentIdx = 0;
		// 等待所有渲染操作完成的特殊Fence
		ZXD3D12Fence* mEndRenderFence;
		// 帧与帧之间同步用的Fence
		vector<ZXD3D12Fence*> mFrameFences;

		ComPtr<IDXGIFactory4> mDXGIFactory;
		ComPtr<ID3D12Device5> mD3D12Device;

		uint32_t mPresentFBOIdx = 0;
		const UINT mPresentBufferCount = 2;
		ComPtr<IDXGISwapChain4> mSwapChain;
		vector<ComPtr<ID3D12Resource>> mPresentBuffers;
		vector<ZXD3D12DescriptorHandle> mPresentBufferRTVHandles;

		ComPtr<ID3D12CommandQueue> mCommandQueue;

		void InitD3D12();
		void GetDeviceProperties();
		void CreateSwapChain();
		void CreateSwapChainBuffers();
	public:
		void InitAfterConstructor();


		/// <summary>
		/// D3D12资源，以及相关创建销毁接口
		/// </summary>
	private:
		vector<ZXD3D12VAO*> mVAOArray;
		vector<ZXD3D12FBO*> mFBOArray;
		vector<ZXD3D12RenderBuffer*> mRenderBufferArray;
		vector<ZXD3D12Texture*> mTextureArray;
		vector<ZXD3D12Pipeline*> mPipelineArray;
		vector<ZXD3D12MaterialData*> mMaterialDataArray;
		vector<ZXD3D12DrawCommand*> mDrawCommandArray;

		map<uint32_t, uint32_t> mMeshsToDelete;
		map<uint32_t, uint32_t> mTexturesToDelete;
		map<uint32_t, uint32_t> mMaterialDatasToDelete;
		map<uint32_t, uint32_t> mShadersToDelete;

		uint32_t GetNextVAOIndex();
		ZXD3D12VAO* GetVAOByIndex(uint32_t idx);
		void DestroyVAOByIndex(uint32_t idx);
		uint32_t GetNextFBOIndex();
		ZXD3D12FBO* GetFBOByIndex(uint32_t idx);
		void DestroyFBOByIndex(uint32_t idx);
		uint32_t GetNextRenderBufferIndex();
		ZXD3D12RenderBuffer* GetRenderBufferByIndex(uint32_t idx);
		void DestroyRenderBufferByIndex(uint32_t idx);
		uint32_t GetNextTextureIndex();
		ZXD3D12Texture* GetTextureByIndex(uint32_t idx);
		void DestroyTextureByIndex(uint32_t idx);
		uint32_t GetNextPipelineIndex();
		ZXD3D12Pipeline* GetPipelineByIndex(uint32_t idx);
		void DestroyPipelineByIndex(uint32_t idx);
		uint32_t GetNextMaterialDataIndex();
		ZXD3D12MaterialData* GetMaterialDataByIndex(uint32_t idx);
		void DestroyMaterialDataByIndex(uint32_t idx);
		uint32_t GetNextDrawCommandIndex();
		ZXD3D12DrawCommand* GetDrawCommandByIndex(uint32_t idx);

		void CheckDeleteData();

		uint32_t CreateZXD3D12Texture(ComPtr<ID3D12Resource>& textureResource, const D3D12_RENDER_TARGET_VIEW_DESC& rtvDesc);
		uint32_t CreateZXD3D12Texture(ComPtr<ID3D12Resource>& textureResource, const D3D12_SHADER_RESOURCE_VIEW_DESC& srvDesc);
		uint32_t CreateZXD3D12Texture(ComPtr<ID3D12Resource>& textureResource, const D3D12_SHADER_RESOURCE_VIEW_DESC& srvDesc, const D3D12_RENDER_TARGET_VIEW_DESC& rtvDesc);
		uint32_t CreateZXD3D12Texture(ComPtr<ID3D12Resource>& textureResource, const D3D12_SHADER_RESOURCE_VIEW_DESC& srvDesc, const D3D12_DEPTH_STENCIL_VIEW_DESC& dsvDesc);
		ZXD3D12Buffer CreateBuffer(UINT64 size, D3D12_RESOURCE_FLAGS flags, D3D12_RESOURCE_STATES initState, D3D12_HEAP_TYPE heapType, bool cpuAddress = false, bool gpuAddress = false, const void* data = nullptr);
		void DestroyBuffer(ZXD3D12Buffer& buffer);


		/// <summary>
		/// D3D12光线追踪相关资源和接口
		/// </summary>
	private:
		// 光追Shader编译库
		IDxcLibrary* mDxcLibrary = nullptr;
		IDxcCompiler* mDxcCompiler = nullptr;
		IDxcIncludeHandler* mDxcIncludeHandler = nullptr;

		// 当前的光线追踪管线ID
		uint32_t mCurRTPipelineID = 0;
		// 光线追踪管线
		vector<ZXD3D12RTPipeline*> mRTPipelines;
		// 构建TLAS的中间Buffer
		vector<ZXD3D12Buffer> mTLASScratchBuffers;
		vector<ZXD3D12Buffer> mTLASInstanceBuffers;

		// 在累积式光追场景中，用来判断画面刷新的数据
		vector<Matrix4> mRTVPMatrix;
		vector<uint32_t> mRTFrameCount;

		// 光追管线Constant Buffer可容纳的32位数据数量
		uint32_t mRT32BitConstantNum = 128;
		// 光追管线Constant Buffer的数据临时缓存地址
		void* mRT32BitConstantBufferAddress = nullptr;

		// 场景中的纹理数量
		uint32_t mRTSceneTextureNum = 100;
		// 场景中的CubeMap数量
		uint32_t mRTSceneCubeMapNum = 10;
		// 场景中的渲染对象数量
		uint32_t mRTSceneRenderObjectNum = 100;

		// 根签名中各参数在描述符堆中的偏移量
		// register(t0, space0) TLAS
		const uint32_t mRTRootParamOffsetInDescriptorHeapTLAS = 0;
		// register(u0, space0) 输出图像
		const uint32_t mRTRootParamOffsetInDescriptorHeapOutputImage = 1;
		// register(t1, space0) 顶点索引Buffer
		const uint32_t mRTRootParamOffsetInDescriptorHeapIndexBuffer = 2;
		// register(t2, space0) 顶点Buffer
		const uint32_t mRTRootParamOffsetInDescriptorHeapVertexBuffer = 3;
		// register(t3, space0) 材质数据Buffer
		const uint32_t mRTRootParamOffsetInDescriptorHeapMaterialData = 4;
		// register(t0, space1) 2D纹理数组
		const uint32_t mRTRootParamOffsetInDescriptorHeapTexture2DArray = 6;
		// register(t0, space2) CubeMap纹理数组
		const uint32_t mRTRootParamOffsetInDescriptorHeapTextureCubeArray = 6 + mRTSceneTextureNum;
		// register(b0, space0) 常量Buffer (Vulkan PushConstants)
		const uint32_t mRTRootParamOffsetInDescriptorHeapConstantBuffer = 5;

		// 当前这一帧要绘制的对象信息数组
		vector<ZXD3D12ASInstanceData> mASInstanceData;
		// 当前场景中所有纹理索引数组
		vector<uint32_t> mCurRTSceneTextureIndexes;
		// 当前场景中所有纹理的索引与纹理数组下标的映射表
		unordered_map<uint32_t, uint32_t> mCurRTSceneTextureIndexMap;
		// 当前场景中所有CubeMap索引数组
		vector<uint32_t> mCurRTSceneCubeMapIndexes;
		// 当前场景中所有CubeMap的索引与CubeMap数组下标的映射表
		unordered_map<uint32_t, uint32_t> mCurRTSceneCubeMapIndexMap;
		// 当前场景中所有光追材质索引数组
		vector<uint32_t> mCurRTSceneRTMaterialDatas;
		// 当前场景中所有光追材质的索引与光追材质数组下标的映射表
		unordered_map<uint32_t, uint32_t> mCurRTSceneRTMaterialDataMap;

		// TLAS Group，一个场景有一个TLAS Group
		vector<ZXD3D12ASGroup*> mTLASGroupArray;
		// 所有的光追材质数组，其中可能包括已销毁的，未在场景中的
		vector<ZXD3D12RTMaterialData*> mRTMaterialDataArray;
		// 准备销毁的光追材质
		unordered_map<uint32_t, uint32_t> rtMaterialDatasToDelete;

		ComPtr<ID3D12RootSignature> mEmptyLocalRootSignature;
		ComPtr<ID3D12RootSignature> mEmptyGlobalRootSignature;

		// 初始化全局的光追相关资源
		void InitDXR();
		void InitEmptyRootSignature();

		uint32_t GetNextTLASGroupIndex();
		ZXD3D12ASGroup* GetTLASGroupByIndex(uint32_t idx);
		void DestroyTLASGroupByIndex(uint32_t idx);
		uint32_t GetNextRTMaterialDataIndex();
		ZXD3D12RTMaterialData* GetRTMaterialDataByIndex(uint32_t idx);
		void DestroyRTMaterialDataByIndex(uint32_t idx);

		ComPtr<IDxcBlob> CompileRTShader(const string& path);
		ZXD3D12DXILLibraryDesc CreateDXILLibrary(const ComPtr<IDxcBlob>& dxilBlob, const vector<wstring>& exportedSymbols);

		void DestroyAccelerationStructure(ZXD3D12AccelerationStructure& accelerationStructure);

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
		bool mWindowResized = false;
		uint32_t mNewWindowWidth = 0;
		uint32_t mNewWindowHeight = 0;

		ZXD3D12Fence* mImmediateExeFence;
		ComPtr<ID3D12CommandAllocator> mImmediateExeAllocator;
		ComPtr<ID3D12GraphicsCommandList4> mImmediateExeCommandList;

		ViewPortInfo mViewPortInfo;

		uint32_t mCurFBOIdx = 0;
		uint32_t mCurPipeLineIdx = 0;
		uint32_t mCurMaterialDataIdx = 0;
		vector<ZXD3D12DrawIndex> mDrawIndexes;

		uint32_t GetCurFrameBufferIndex();

		void DoWindowSizeChange();

		ZXD3D12Fence* CreateZXD3D12Fence();
		void SignalFence(ZXD3D12Fence* fence);
		void WaitForFence(ZXD3D12Fence* fence);

		void* GetShaderPropertyAddress(ShaderReference* reference, uint32_t materialDataID, const string& name, uint32_t idx = 0);
		vector<void*> GetShaderPropertyAddressAllBuffer(ShaderReference* reference, uint32_t materialDataID, const string& name, uint32_t idx = 0);

		array<const CD3DX12_STATIC_SAMPLER_DESC, 4> GetStaticSamplersDesc();
		void InitImmediateExecution();
		void ImmediatelyExecute(std::function<void(ComPtr<ID3D12GraphicsCommandList4> cmdList)>&& function);
	};
}