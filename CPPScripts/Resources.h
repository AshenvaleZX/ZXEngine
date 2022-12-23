#pragma once
#include "pubh.h"

namespace ZXEngine
{
	struct TextureStruct
	{
		string uniformName;
		string path;
	};

	struct MaterialStruct
	{
		string name;
		string shaderPath;
		vector<TextureStruct*> textures;
	};

	struct PrefabStruct
	{
		string name;
		unsigned int layer;
		list<json> components;
	};

	struct SceneStruct
	{
		vector<string> skyBox;
		list<PrefabStruct*> prefabs;
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
		static SceneStruct* LoadScene(string path);
		static PrefabStruct* LoadPrefab(string path, bool isBuiltIn = false);
		static MaterialStruct* LoadMaterial(string path, bool isBuiltIn = false);
		static vector<string> LoadCubeMap(json data, bool isBuiltIn = false);

		static string JsonStrToString(json data);

	private:
		static string assetsPath;
		static const string builtInAssetsPath;

		static json GetAssetData(string path, bool isBuiltIn = false);
	};
}