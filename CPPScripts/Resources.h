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
		string shaderPath;
		vector<TextureStruct*> textures;
	};

	struct PrefabStruct
	{
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
		static string GetAssetsPath();
		static string GetAssetFullPath(string path);
		static json LoadJson(string path);
		static SceneStruct* LoadScene(string path);
		static PrefabStruct* LoadPrefab(string path);
		static MaterialStruct* LoadMaterial(string path);
		static vector<string> LoadCubeMap(json data);

		static string JsonStrToString(json data);

	private:
		static string assetsPath;

		static json GetAssetData(string path);
	};
}