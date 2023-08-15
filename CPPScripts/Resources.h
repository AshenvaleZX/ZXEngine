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
		string shaderPath;        // ��դ�����ߵ�shader·��
		uint32_t hitGroupIdx = 0; // ��׷���ߵ�hitGroup����

		map<string, float> floatDatas;
		map<string, uint32_t> uintDatas;
		map<string, Vector2> vec2Datas;
		map<string, Vector3> vec3Datas;
		map<string, Vector4> vec4Datas;

		vector<TextureStruct*> textures;
		vector<CubeMapStruct*> cubeMaps;

		MaterialType type = MaterialType::Rasterization;

		~MaterialStruct() { for (auto iter : textures) delete iter; }
	};

	struct PrefabStruct
	{
		string name;
		uint32_t layer = 0;
		list<json> components;
		PrefabStruct* parent = nullptr;
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

		static void SetAssetsPath(const string& path);
		// ��ȡ��ǰ����Assets�ļ���·��
		static string GetAssetsPath();
		// ��Assets�µ�·��תΪӲ���ϵ�·��
		static string GetAssetFullPath(const string& path, bool isBuiltIn = false);
		// ��Ӳ���ϵ�·��תΪAssets�µ�·��
		static string GetAssetLocalPath(const string& path);
		static string GetAssetName(const string& path);
		static string GetAssetExtension(const string& path);

		static json LoadJson(const string& path);
		static string JsonStrToString(const json& data);
		static string LoadTextFile(const string& path);
		static vector<char> LoadBinaryFile(const string& path);

		static SceneStruct* LoadScene(const string& path);
		static PrefabStruct* LoadPrefab(const string& path, bool isBuiltIn = false);
		static MaterialStruct* LoadMaterial(const string& path, bool isBuiltIn = false);
		static vector<string> LoadCubeMap(const json& data, bool isBuiltIn = false);


	private:
		static string assetsPath;
		static const string builtInAssetsPath;

		static json GetAssetData(const string& path, bool isBuiltIn = false);
		static PrefabStruct* ParsePrefab(json data);
	};
}