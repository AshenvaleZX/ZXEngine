#pragma once
#include "pubh.h"

namespace ZXEngine
{
	struct CameraStruct
	{

	};

	struct MaterialStruct
	{
		string shaderPath;
	};

	struct PrefabStruct
	{
		list<json> components;
	};

	struct SceneStruct
	{
		list<CameraStruct*> cameras;
		list<PrefabStruct*> prefabs;
	};

	class Resources
	{
	public:
		Resources() {};
		~Resources() {};

		static void SetAssetsPath(const char* path);
		static string GetAssetFullPath(const char* path);
		static SceneStruct* LoadScene(const char* path);
		static PrefabStruct* LoadPrefab(const char* path);
		static MaterialStruct* LoadMaterial(const char* path);

	private:
		static const char* assetsPath;

		static json GetAssetData(const char* path);
	};
}