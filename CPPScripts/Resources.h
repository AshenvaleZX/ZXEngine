#pragma once
#include "pubh.h"
#include "PublicStruct.h"
#include <stb_image.h>

namespace ZXEngine
{
	struct TextureStruct
	{
		string uniformName;
		string path;
		TextureFullData* data = nullptr;

		~TextureStruct();
	};

	struct CubeMapStruct
	{
		string uniformName;
		vector<string> paths;
		CubeMapFullData* data = nullptr;

		~CubeMapStruct();
	};

	struct MaterialStruct
	{
		string name;
		string path;
		string shaderPath;        // 光栅化管线的shader路径
		string shaderCode;
		uint32_t hitGroupIdx = 0; // 光追管线的hitGroup索引

		map<string, float> floatDatas;
		map<string, uint32_t> uintDatas;
		map<string, Vector2> vec2Datas;
		map<string, Vector3> vec3Datas;
		map<string, Vector4> vec4Datas;
		map<string, Vector4> colorDatas;

		vector<TextureStruct*> textures;
		vector<CubeMapStruct*> cubeMaps;

		MaterialType type = MaterialType::Forward;

		~MaterialStruct();
	};

	struct PrefabStruct
	{
		string name;
		uint32_t layer = 0;
		list<json> components;
		PrefabStruct* parent = nullptr;
		vector<PrefabStruct*> children;

		ModelData* modelData = nullptr;
		MaterialStruct* material = nullptr;

		~PrefabStruct();
	};

	struct SceneStruct
	{
		vector<string> skyBox;
		vector<PrefabStruct*> prefabs;
		RenderPipelineType renderPipelineType = RenderPipelineType::Forward;
		RayTracingShaderPathGroup rtShaderPathGroup;

		~SceneStruct();
	};

	struct PrefabLoadHandle
	{
		std::future<PrefabStruct*> future;
		std::function<void(PrefabStruct*)> callback;
	};

	struct MaterialLoadHandle
	{
		std::future<MaterialStruct*> future;
		std::function<void(MaterialStruct*)> callback;
	};

	struct ModelDataLoadHandle
	{
		std::future<ModelData*> future;
		std::function<void(ModelData*)> callback;
	};

	class Resources
	{
		// -------- 同步接口 --------
	public:
		static void SetAssetsPath(const string& path);
		// 获取当前工程Assets文件夹路径
		static string GetAssetsPath();
		// 把Assets下的路径转为硬盘上的路径
		static string GetAssetFullPath(const string& path, bool isBuiltIn = false);
		// 把硬盘上的路径转为Assets下的路径
		static string GetAssetLocalPath(const string& path);
		static string GetAssetName(const string& path);
		static string GetAssetExtension(const string& path);
		static string GetAssetNameWithExtension(const string& path);

		static json LoadJson(const string& path);
		static string JsonStrToString(const json& data);
		static string LoadTextFile(const string& path, bool logError = true);
		static bool LoadBinaryFile(vector<unsigned char>& data, const string& path);
		static unsigned char* LoadTexture(const string& path, int* width, int* height, int* components, int channels);
		static void DeleteTexture(unsigned char* data);

		static SceneStruct* LoadScene(const string& path);
		static PrefabStruct* LoadPrefab(const string& path, bool isBuiltIn = false, bool async = false);
		static MaterialStruct* LoadMaterial(const string& path, bool isBuiltIn = false);
		static vector<string> LoadCubeMap(const json& data, bool isBuiltIn = false);

		static TextureFullData* LoadTextureFullData(const string& path, bool isBuiltIn = false);
		static CubeMapFullData* LoadCubeMapFullData(const vector<string>& paths, bool isBuiltIn = false);

	private:
		static string mAssetsPath;

		static json GetAssetData(const string& path, bool isBuiltIn = false);
		static PrefabStruct* ParsePrefab(json data, bool async = false, bool isBuiltIn = false);


		// -------- 异步接口 --------
	public:
		static void CheckAsyncLoad();
		static void ClearAsyncLoad();
		static void AsyncLoadPrefab(const string& path, std::function<void(PrefabStruct*)> callback, bool isBuiltIn = false);
		static void AsyncLoadMaterial(const string& path, std::function<void(MaterialStruct*)> callback, bool isBuiltIn = false, bool isEditor = false);
		static void AsyncLoadModelData(const string& path, std::function<void(ModelData*)> callback, bool isBuiltIn = false, bool isEditor = false);
#ifdef ZX_EDITOR
		static void ClearEditorAsyncLoad();
#endif

	private:
		static vector<PrefabLoadHandle> mPrefabLoadHandles;
		static vector<PrefabLoadHandle> mDiscardedPrefabLoadHandles;
		static vector<MaterialLoadHandle> mMaterialLoadHandles;
		static vector<MaterialLoadHandle> mDiscardedMaterialLoadHandles;
		static vector<ModelDataLoadHandle> mModelDataLoadHandles;
		static vector<ModelDataLoadHandle> mDiscardedModelDataLoadHandles;
#ifdef ZX_EDITOR
		static vector<MaterialLoadHandle> mEditorMaterialLoadHandles;
		static vector<MaterialLoadHandle> mDiscardedEditorMaterialLoadHandles;
		static vector<ModelDataLoadHandle> mEditorModelDataLoadHandles;
		static vector<ModelDataLoadHandle> mDiscardedEditorModelDataLoadHandles;
#endif

		static void DoAsyncLoadPrefab(std::promise<PrefabStruct*>&& prms, string path, bool isBuiltIn);
		static void DoAsyncLoadMaterial(std::promise<MaterialStruct*>&& prms, string path, bool isBuiltIn);
		static void DoAsyncLoadModelData(std::promise<ModelData*>&& prms, string path, bool isBuiltIn);
	};
}