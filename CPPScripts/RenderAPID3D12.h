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
		/// ��׼RenderAPI�ӿ�
		/// </summary>
	public:
		RenderAPID3D12();
		~RenderAPID3D12() {};

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

		// Shader����
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
		/// ��׼RayTracing�ӿ�(��δʵ�ֻ���DXR�Ĺ���׷��)
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
		virtual void SetRayTracingSkyBox(uint32_t textureID);
		virtual void PushRayTracingMaterialData(Material* material);
		virtual void PushAccelerationStructure(uint32_t VAO, uint32_t hitGroupIdx, uint32_t rtMaterialDataID, const Matrix4& transform);

		// Ray Trace
		virtual void RayTrace(uint32_t commandID, const RayTracingPipelineConstants& rtConstants);

		// Acceleration Structure
		virtual void BuildTopLevelAccelerationStructure(uint32_t commandID);
		virtual void BuildBottomLevelAccelerationStructure(uint32_t VAO, bool isCompact);


		/// <summary>
		/// ������ʱһ���Գ�ʼ���ĺ���D3D12�������ر���
		/// </summary>
	private:
		// ��������������С
		UINT mRtvDescriptorSize = 0;
		UINT mDsvDescriptorSize = 0;
		UINT mCbvSrvUavDescriptorSize = 0;
		// ��Ⱦʱ��̬�������������
		vector<UINT> mDynamicDescriptorOffsets;
		vector<ComPtr<ID3D12DescriptorHeap>> mDynamicDescriptorHeaps;

		// ��Ļ��̨������ͼ���ʽ
		DXGI_FORMAT mPresentBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
		// Ĭ�ϵ������FrameBufferɫ�ʿռ�
		const DXGI_FORMAT mDefaultImageFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
		// 4X MSAA�����ȼ�
		UINT m4xMSAAQuality = 0;
		UINT msaaSamplesCount = 4;

		// ��ǰ��DX_MAX_FRAMES_IN_FLIGHT�еĵڼ�֡
		uint32_t mCurrentFrame = 0;
		// ��ǰ��һ֡Ҫд���Present Buffer�±�
		uint32_t mCurPresentIdx = 0;
		// �ȴ�������Ⱦ������ɵ�����Fence
		ZXD3D12Fence* mEndRenderFence;
		// ֡��֮֡��ͬ���õ�Fence
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
		/// D3D12��Դ���Լ���ش������ٽӿ�
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
		/// D3D12����׷�������Դ�ͽӿ�
		/// </summary>
	private:
		// ��׷Shader�����
		IDxcLibrary* mDxcLibrary = nullptr;
		IDxcCompiler* mDxcCompiler = nullptr;
		IDxcIncludeHandler* mDxcIncludeHandler = nullptr;

		// ��ǰ�Ĺ���׷�ٹ���ID
		uint32_t mCurRTPipelineID = 0;
		// ����׷�ٹ���
		vector<ZXD3D12RTPipeline*> mRTPipelines;
		// ����TLAS���м�Buffer
		vector<ZXD3D12Buffer> mTLASScratchBuffers;
		vector<ZXD3D12Buffer> mTLASInstanceBuffers;

		// ���ۻ�ʽ��׷�����У������жϻ���ˢ�µ�����
		vector<Matrix4> mRTVPMatrix;
		vector<uint32_t> mRTFrameCount;

		// ��׷����Constant Buffer�����ɵ�32λ��������
		uint32_t mRT32BitConstantNum = 128;
		// ��׷����Constant Buffer��������ʱ�����ַ
		void* mRT32BitConstantBufferAddress = nullptr;

		// �����е���������
		uint32_t mRTSceneTextureNum = 100;
		// �����е�CubeMap����
		uint32_t mRTSceneCubeMapNum = 10;
		// �����е���Ⱦ��������
		uint32_t mRTSceneRenderObjectNum = 100;

		// ��ǩ���и����������������е�ƫ����
		// register(t0, space0) TLAS
		const uint32_t mRTRootParamOffsetInDescriptorHeapTLAS = 0;
		// register(u0, space0) ���ͼ��
		const uint32_t mRTRootParamOffsetInDescriptorHeapOutputImage = 1;
		// register(t1, space0) ��������Buffer
		const uint32_t mRTRootParamOffsetInDescriptorHeapIndexBuffer = 2;
		// register(t2, space0) ����Buffer
		const uint32_t mRTRootParamOffsetInDescriptorHeapVertexBuffer = 3;
		// register(t3, space0) ��������Buffer
		const uint32_t mRTRootParamOffsetInDescriptorHeapMaterialData = 4;
		// register(t0, space1) 2D��������
		const uint32_t mRTRootParamOffsetInDescriptorHeapTexture2DArray = 6;
		// register(t0, space2) CubeMap��������
		const uint32_t mRTRootParamOffsetInDescriptorHeapTextureCubeArray = 6 + mRTSceneTextureNum;
		// register(b0, space0) ����Buffer (Vulkan PushConstants)
		const uint32_t mRTRootParamOffsetInDescriptorHeapConstantBuffer = 5;

		// ��ǰ��һ֡Ҫ���ƵĶ�����Ϣ����
		vector<ZXD3D12ASInstanceData> mASInstanceData;
		// ��ǰ����������������������
		vector<uint32_t> mCurRTSceneTextureIndexes;
		// ��ǰ������������������������������±��ӳ���
		unordered_map<uint32_t, uint32_t> mCurRTSceneTextureIndexMap;
		// ��ǰ����������CubeMap��������
		vector<uint32_t> mCurRTSceneCubeMapIndexes;
		// ��ǰ����������CubeMap��������CubeMap�����±��ӳ���
		unordered_map<uint32_t, uint32_t> mCurRTSceneCubeMapIndexMap;
		// ��ǰ���������й�׷������������
		vector<uint32_t> mCurRTSceneRTMaterialDatas;
		// ��ǰ���������й�׷���ʵ��������׷���������±��ӳ���
		unordered_map<uint32_t, uint32_t> mCurRTSceneRTMaterialDataMap;

		// TLAS Group��һ��������һ��TLAS Group
		vector<ZXD3D12ASGroup*> mTLASGroupArray;
		// ���еĹ�׷�������飬���п��ܰ��������ٵģ�δ�ڳ����е�
		vector<ZXD3D12RTMaterialData*> mRTMaterialDataArray;
		// ׼�����ٵĹ�׷����
		unordered_map<uint32_t, uint32_t> rtMaterialDatasToDelete;

		ComPtr<ID3D12RootSignature> mEmptyLocalRootSignature;
		ComPtr<ID3D12RootSignature> mEmptyGlobalRootSignature;

		// ��ʼ��ȫ�ֵĹ�׷�����Դ
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
		/// ���������ӿ�
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