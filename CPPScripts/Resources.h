#pragma once
#include "pubh.h"
#include "PublicStruct.h"

namespace ZXEngine
{
	struct TextureStruct
	{
		string uniformName;
		string path;
	};

	struct CubeMapStruct
	{
		string uniformName;
		vector<string> paths;
	};

	struct MaterialStruct
	{
		string name;
		string path;
		string shaderPath;
		vector<TextureStruct*> textures;
		vector<CubeMapStruct*> cubeMaps;

		~MaterialStruct() { for (auto iter : textures) delete iter; }
	};

	struct PrefabStruct
	{
		string name;
		unsigned int layer;
		list<json> components;
		PrefabStruct* parent;
		vector<PrefabStruct*> children;

		~PrefabStruct() { for (auto iter : children) delete iter; }
	};

	struct SceneStruct
	{
		vector<string> skyBox;
		vector<PrefabStruct*> prefabs;
		RenderPipelineType renderPipelineType = RenderPipelineType::Rasterization;
		RayTracingShaderPathGroup rtShaderPathGroup;

		~SceneStruct() { for (auto iter : prefabs) delete iter; }
	};

	class Resources
	{
	public:
		Resources() {};
		~Resources() {};

		static void SetAssetsPath(string path);
		// 获取当前工程Assets文件夹路径
		static string GetAssetsPath();
		// 把Assets下的路径转为硬盘上的路径
		static string GetAssetFullPath(string path, bool isBuiltIn = false);
		// 把硬盘上的路径转为Assets下的路径
		static string GetAssetLocalPath(string path);
		static string GetAssetName(string path);

		static json LoadJson(string path);
		static string JsonStrToString(json data);
		static string LoadTextFile(string path);
		static vector<char> LoadBinaryFile(string path);

		static SceneStruct* LoadScene(string path);
		static PrefabStruct* LoadPrefab(string path, bool isBuiltIn = false);
		static MaterialStruct* LoadMaterial(string path, bool isBuiltIn = false);
		static vector<string> LoadCubeMap(json data, bool isBuiltIn = false);


	private:
		static string assetsPath;
		static const string builtInAssetsPath;

		static json GetAssetData(string path, bool isBuiltIn = false);
		static PrefabStruct* ParsePrefab(json data);
	};
}