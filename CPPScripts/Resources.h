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
		// ��ȡ��ǰ����Assets�ļ���·��
		static string GetAssetsPath();
		// ��Assets�µ�·��תΪӲ���ϵ�·��
		static string GetAssetFullPath(string path, bool isBuiltIn = false);
		// ��Ӳ���ϵ�·��תΪAssets�µ�·��
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