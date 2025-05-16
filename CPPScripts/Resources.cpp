#include "Resources.h"
#include "ModelUtil.h"
#include "ProjectSetting.h"

#if defined(ZX_PLATFORM_ANDROID)
#include "GlobalData.h"
#include <android/asset_manager.h>
#elif defined(ZX_PLATFORM_IOS)
#include "iOS/iOSUtil.h"
#endif

namespace ZXEngine
{
	string Resources::mAssetsPath;
	vector<PrefabLoadHandle> Resources::mPrefabLoadHandles;
	vector<PrefabLoadHandle> Resources::mDiscardedPrefabLoadHandles;
	vector<MaterialLoadHandle> Resources::mMaterialLoadHandles;
	vector<MaterialLoadHandle> Resources::mDiscardedMaterialLoadHandles;
	vector<ModelDataLoadHandle> Resources::mModelDataLoadHandles;
	vector<ModelDataLoadHandle> Resources::mDiscardedModelDataLoadHandles;
#ifdef ZX_EDITOR
	vector<MaterialLoadHandle> Resources::mEditorMaterialLoadHandles;
	vector<MaterialLoadHandle> Resources::mDiscardedEditorMaterialLoadHandles;
	vector<ModelDataLoadHandle> Resources::mEditorModelDataLoadHandles;
	vector<ModelDataLoadHandle> Resources::mDiscardedEditorModelDataLoadHandles;
#endif

	static const vector<string> mBuiltInSkyBoxPath =
	{
		"Textures/BrightSky/right.png",
		"Textures/BrightSky/left.png",
		"Textures/BrightSky/up.png",
		"Textures/BrightSky/down.png",
		"Textures/BrightSky/front.png",
		"Textures/BrightSky/back.png"
	};

	TextureStruct::~TextureStruct()
	{
		if (data) delete data;
	}

	CubeMapStruct::~CubeMapStruct()
	{
		if (data) delete data;
	}

	MaterialStruct::~MaterialStruct()
	{
		for (auto iter : textures)
			delete iter;

		for (auto iter : cubeMaps)
			delete iter;
	}

	PrefabStruct::~PrefabStruct()
	{
		if (material)
			delete material;

		if (modelData)
			delete modelData;

		for (auto iter : children)
			delete iter;
	}

	SceneStruct::~SceneStruct()
	{
		for (auto iter : prefabs)
			delete iter;
	}

	void Resources::SetAssetsPath(const string& path)
	{
		mAssetsPath = path;
	}

	string Resources::GetAssetsPath()
	{
		return mAssetsPath;
	}

	string Resources::GetAssetFullPath(const string& path, bool isBuiltIn)
	{
		if (isBuiltIn)
			return ProjectSetting::mBuiltInAssetsPath + path;
		else
			return mAssetsPath + path;
	}

	string Resources::GetAssetLocalPath(const string& path)
	{
		size_t s = path.find("Assets");
		if (s > 0)
			return path.substr(s + 7, path.length() - s - 7);
		else
			return path;
	}

	string Resources::GetAssetName(const string& path)
	{
		size_t s = path.rfind("/");
		size_t e = path.rfind(".");
		return path.substr(s + 1, e - s - 1);
	}

	string Resources::GetAssetExtension(const string& path)
	{
		size_t s = path.rfind(".");
		return path.substr(s + 1, path.length() - s - 1);
	}

	string Resources::GetAssetWithOutExtension(const string& path)
	{
		size_t e = path.rfind(".");
		return path.substr(0, e);
	}

	string Resources::GetAssetNameWithExtension(const string& path)
	{
		size_t s = path.rfind("/");
		return path.substr(s + 1, path.length() - s - 1);
	}

	string Resources::JsonStrToString(const json& data)
	{
		return data.template get<string>();
	}

	bool Resources::LoadJson(json& data, const string& path)
	{
#if defined(ZX_PLATFORM_DESKTOP) || defined(ZX_PLATFORM_IOS)
#if defined(ZX_PLATFORM_IOS)
		string loadPath = iOSUtil::GetResourcePath(GetAssetWithOutExtension(path), GetAssetExtension(path));
		if (loadPath.empty())
		{
			Debug::LogError("Get iOS asset path failed: " + path);
			return false;
		}
#else
		string loadPath = path;
#endif
		std::ifstream f(loadPath);
		if (!f.is_open())
		{
			Debug::LogError("Load json asset failed: " + path);
			return false;
		}

		try
		{
			data = json::parse(f);
		}
		catch (json::exception& e)
		{
			Debug::LogError("Asset format error: " + path);
			string msg = e.what();
			Debug::LogError("Error detail: " + msg);
			return false;
		}
#elif defined(ZX_PLATFORM_ANDROID)
		AAssetManager* assetManager = GlobalData::app->activity->assetManager;
		AAsset* asset = AAssetManager_open(assetManager, path.c_str(), AASSET_MODE_BUFFER);
		if (!asset)
		{
			Debug::LogError("Load json asset failed: " + path);
			return false;
		}

		off_t size = AAsset_getLength(asset);
		char* buffer = new char[size + 1];
		AAsset_read(asset, buffer, size);
		buffer[size] = '\0';
		AAsset_close(asset);

		try
		{
			data = json::parse(buffer);
		}
		catch (json::exception& e)
		{
			Debug::LogError("Asset format error: " + path);
			string msg = e.what();
			Debug::LogError("Error detail: " + msg);
			return false;
		}

		delete[] buffer;
#endif

		return true;
	}

	string Resources::LoadTextFile(const string& path, bool logError)
	{
		string text = "";

#if defined(ZX_PLATFORM_DESKTOP) || defined(ZX_PLATFORM_IOS)
#if defined(ZX_PLATFORM_IOS)
		string loadPath = iOSUtil::GetResourcePath(GetAssetWithOutExtension(path), GetAssetExtension(path));
		if (loadPath.empty())
		{
			if (logError)
				Debug::LogError("Get iOS asset path failed: " + path);
			return "";
		}
#else
		string loadPath = path;
#endif
		ifstream file;
		file.exceptions(ifstream::failbit | ifstream::badbit);

		try
		{
			file.open(loadPath);
			stringstream stream;
			stream << file.rdbuf();
			file.close();
			text = stream.str();
		}
		catch (ifstream::failure e)
		{
			if (logError)
				Debug::LogError("Load text asset failed: " + path);
		}
#elif defined(ZX_PLATFORM_ANDROID)
		AAssetManager* assetManager = GlobalData::app->activity->assetManager;
		AAsset* asset = AAssetManager_open(assetManager, path.c_str(), AASSET_MODE_BUFFER);
		if (!asset)
		{
			if (logError)
				Debug::LogError("Load text asset failed: " + path);
			return "";
		}

		off_t size = AAsset_getLength(asset);
		char* buffer = new char[size + 1];
		AAsset_read(asset, buffer, size);
		buffer[size] = '\0';
		AAsset_close(asset);

		text = buffer;
		delete[] buffer;
#endif

		return text;
	}

	bool Resources::LoadBinaryFile(vector<unsigned char>& data, const string& path)
	{
#if defined(ZX_PLATFORM_DESKTOP) || defined(ZX_PLATFORM_IOS)
#if defined(ZX_PLATFORM_IOS)
		string loadPath = iOSUtil::GetResourcePath(GetAssetWithOutExtension(path), GetAssetExtension(path));
		if (loadPath.empty())
		{
			Debug::LogError("Get iOS asset path failed: " + path);
			return false;
		}
#else
		string loadPath = path;
#endif
		// ate:在文件末尾开始读取，从文件末尾开始读取的优点是我们可以使用读取位置来确定文件的大小并分配缓冲区
		ifstream file(loadPath, std::ios::ate | std::ios::binary);
		if (!file.is_open())
		{
			Debug::LogError("Load binary asset failed: " + path);
			return false;
		}

		// 使用读取位置来确定文件的大小并分配缓冲区
		size_t fileSize = (size_t)file.tellg();
		data.resize(fileSize);

		// 返回文件开头，真正读取内容
		file.seekg(0);
		file.read(reinterpret_cast<char*>(data.data()), fileSize);
		file.close();

		return true;
#elif defined(ZX_PLATFORM_ANDROID)
		AAssetManager* assetManager = GlobalData::app->activity->assetManager;
		AAsset* asset = AAssetManager_open(assetManager, path.c_str(), AASSET_MODE_BUFFER);
		if (!asset)
		{
			Debug::LogError("Load binary asset failed: " + path);
			return false;
		}

		off_t size = AAsset_getLength(asset);
		data.resize(size);
		AAsset_read(asset, data.data(), size);
		AAsset_close(asset);

		return true;
#endif
	}

	unsigned char* Resources::LoadTexture(const string& path, int* width, int* height, int* components, int channels)
	{
		vector<unsigned char> textureData;
		Resources::LoadBinaryFile(textureData, path);
		return stbi_load_from_memory(textureData.data(),
			static_cast<int>(textureData.size()),
			width, height, components, channels
		);
	}

	void Resources::DeleteTexture(unsigned char* data)
	{
		stbi_image_free(data);
	}

	bool Resources::GetAssetData(json& data, const string& path, bool isBuiltIn)
	{
		if (!isBuiltIn)
		{
			Debug::Log("Load asset: " + path);
		}

		string p = Resources::GetAssetFullPath(path, isBuiltIn);
		
		return Resources::LoadJson(data, p);
	}

	SceneStruct* Resources::LoadScene(const string& path)
	{
		json data;
		if (!Resources::GetAssetData(data, path))
			return nullptr;

		SceneStruct* scene = new SceneStruct();

		if (data["SkyBox"].is_null())
		{
			for (size_t i = 0; i < 6; i++)
				scene->skyBox.push_back(Resources::GetAssetFullPath(mBuiltInSkyBoxPath[i], true));
		}
		else
		{
			scene->skyBox = Resources::LoadCubeMap(data["SkyBox"]);
		}

		if (!data["RenderPipelineType"].is_null())
			scene->renderPipelineType = data["RenderPipelineType"];

#ifdef ZX_API_OPENGL
		if (scene->renderPipelineType == RenderPipelineType::RayTracing)
			return scene;
#else
		if (scene->renderPipelineType == RenderPipelineType::RayTracing && !ProjectSetting::isSupportRayTracing)
			return scene;
#endif

		// 临时切换一下渲染管线类型，加载完prefab后再切回来
		auto curPipelineType = ProjectSetting::renderPipelineType;
		ProjectSetting::renderPipelineType = scene->renderPipelineType;
		for (size_t i = 0; i < data["GameObjects"].size(); i++)
		{
			string p = Resources::JsonStrToString(data["GameObjects"][i]);
			PrefabStruct* prefab = Resources::LoadPrefab(p);
			scene->prefabs.push_back(prefab);
		}
		ProjectSetting::renderPipelineType = curPipelineType;

		if (scene->renderPipelineType == RenderPipelineType::RayTracing && !data["RayTracingShaderGroups"].is_null())
		{
			if (!data["RayTracingShaderGroups"]["RayGen"].is_null())
				for (size_t i = 0; i < data["RayTracingShaderGroups"]["RayGen"].size(); i++)
					scene->rtShaderPathGroup.rGenPaths.push_back(Resources::JsonStrToString(data["RayTracingShaderGroups"]["RayGen"][i]));

			if (!data["RayTracingShaderGroups"]["Miss"].is_null())
				for (size_t i = 0; i < data["RayTracingShaderGroups"]["Miss"].size(); i++)
					scene->rtShaderPathGroup.rMissPaths.push_back(Resources::JsonStrToString(data["RayTracingShaderGroups"]["Miss"][i]));

			if (!data["RayTracingShaderGroups"]["HitGroups"].is_null())
			{
				for (size_t i = 0; i < data["RayTracingShaderGroups"]["HitGroups"].size(); i++)
				{
					RayTracingHitGroupPath hitGroupPath;

					if (!data["RayTracingShaderGroups"]["HitGroups"][i]["ClosestHit"].is_null())
						hitGroupPath.rClosestHitPath = Resources::JsonStrToString(data["RayTracingShaderGroups"]["HitGroups"][i]["ClosestHit"]);
					if (!data["RayTracingShaderGroups"]["HitGroups"][i]["AnyHit"].is_null())
						hitGroupPath.rAnyHitPath = Resources::JsonStrToString(data["RayTracingShaderGroups"]["HitGroups"][i]["AnyHit"]);
					if (!data["RayTracingShaderGroups"]["HitGroups"][i]["Intersection"].is_null())
						hitGroupPath.rIntersectionPath = Resources::JsonStrToString(data["RayTracingShaderGroups"]["HitGroups"][i]["Intersection"]);

					scene->rtShaderPathGroup.rHitGroupPaths.push_back(std::move(hitGroupPath));
				}
			}
		}

		return scene;
	}

	PrefabStruct* Resources::LoadPrefab(const string& path, bool isBuiltIn, bool async)
	{
		json data;
		Resources::GetAssetData(data, path, isBuiltIn);
		return ParsePrefab(data, async, isBuiltIn);
	}

	PrefabStruct* Resources::ParsePrefab(json data, bool async, bool isBuiltIn)
	{
		PrefabStruct* prefab = new PrefabStruct;

		prefab->name = data["Name"];
		if (data["Layer"].is_null())
			prefab->layer = static_cast<uint32_t>(GameObjectLayer::Default);
		else
			prefab->layer = data["Layer"];

		for (unsigned int i = 0; i < data["Components"].size(); i++)
		{
			json component = data["Components"][i];

			if (component["Type"] == "MeshRenderer")
			{
				// 材质
				string p = Resources::JsonStrToString(component["Material"]);
				prefab->material = Resources::LoadMaterial(p, isBuiltIn);

				// 模型
				if (!component["Mesh"].is_null())
				{
					p = Resources::JsonStrToString(component["Mesh"]);
					p = Resources::GetAssetFullPath(p);

					prefab->modelData = ModelUtil::LoadModel(p, true, async);
				}
			}

			prefab->components.push_back(component);
		}

		if (!data["GameObjects"].is_null())
		{
			for (unsigned int i = 0; i < data["GameObjects"].size(); i++)
			{
				const json& subData = data["GameObjects"][i];
				auto subPrefab = ParsePrefab(subData, async, isBuiltIn);
				subPrefab->parent = prefab;
				prefab->children.push_back(subPrefab);
			}
		}

		return prefab;
	}

	MaterialStruct* Resources::LoadMaterial(const string& path, bool isBuiltIn)
	{
		MaterialStruct* matStruct = new MaterialStruct;
		matStruct->path = path;
		matStruct->name = GetAssetName(path);

		json data;
		Resources::GetAssetData(data, path, isBuiltIn);

		if (data["Type"] == "Forward")
			matStruct->type = MaterialType::Forward;
		else if (data["Type"] == "Deferred")
			matStruct->type = MaterialType::Deferred;
		else if (data["Type"] == "RayTracing")
			matStruct->type = MaterialType::RayTracing;

		if (matStruct->type == MaterialType::Forward)
		{
			string p = Resources::JsonStrToString(data["Shader"]);
			matStruct->shaderPath = Resources::GetAssetFullPath(p, isBuiltIn);
			matStruct->shaderCode = Resources::LoadTextFile(matStruct->shaderPath);
		}
		else if (matStruct->type == MaterialType::RayTracing)
		{
			if (!data["HitGroup"].is_null())
				matStruct->hitGroupIdx = data["HitGroup"];
		}

		for (size_t i = 0; i < data["Float"].size(); i++)
		{
			matStruct->floatDatas[data["Float"][i]["UniformName"]] = data["Float"][i]["Value"];
		}

		for (size_t i = 0; i < data["UInt"].size(); i++)
		{
			matStruct->uintDatas[data["UInt"][i]["UniformName"]] = data["UInt"][i]["Value"];
		}

		for (size_t i = 0; i < data["Vector2"].size(); i++)
		{
			const json& vec2 = data["Vector2"][i];
			Vector2 value = Vector2(vec2["Value"][0], vec2["Value"][1]);
			matStruct->vec2Datas[vec2["UniformName"]] = value;
		}

		for (size_t i = 0; i < data["Vector3"].size(); i++)
		{
			const json& vec3 = data["Vector3"][i];
			Vector3 value = Vector3(vec3["Value"][0], vec3["Value"][1], vec3["Value"][2]);
			matStruct->vec3Datas[vec3["UniformName"]] = value;
		}

		for (size_t i = 0; i < data["Vector4"].size(); i++)
		{
			const json& vec4 = data["Vector4"][i];
			Vector4 value = Vector4(vec4["Value"][0], vec4["Value"][1], vec4["Value"][2], vec4["Value"][3]);
			matStruct->vec4Datas[vec4["UniformName"]] = value;
		}

		for (size_t i = 0; i < data["Color"].size(); i++)
		{
			const json& color = data["Color"][i];
			Vector4 value = Vector4(color["Value"][0], color["Value"][1], color["Value"][2], color["Value"][3]);
			matStruct->colorDatas[color["UniformName"]] = value;
		}

		for (size_t i = 0; i < data["Textures"].size(); i++)
		{
			const json& texture = data["Textures"][i];

			TextureStruct* textureStruct = new TextureStruct();
			textureStruct->path = Resources::GetAssetFullPath(Resources::JsonStrToString(texture["Path"]), isBuiltIn);
			textureStruct->uniformName = Resources::JsonStrToString(texture["UniformName"]);
			textureStruct->data = Resources::LoadTextureFullData(textureStruct->path, isBuiltIn);

			matStruct->textures.push_back(textureStruct);
		}

		for (size_t i = 0; i < data["CubeMaps"].size(); i++)
		{
			const json& cubeMap = data["CubeMaps"][i];

			CubeMapStruct* cubeMapStruct = new CubeMapStruct();
			cubeMapStruct->paths = Resources::LoadCubeMap(cubeMap, isBuiltIn);
			cubeMapStruct->uniformName = Resources::JsonStrToString(cubeMap["UniformName"]);
			cubeMapStruct->data = Resources::LoadCubeMapFullData(cubeMapStruct->paths, isBuiltIn);

			matStruct->cubeMaps.push_back(cubeMapStruct);
		}

		return matStruct;
	}

	vector<string> Resources::LoadCubeMap(const json& data, bool isBuiltIn)
	{
		vector<string> cube;
		cube.push_back(Resources::GetAssetFullPath(Resources::JsonStrToString(data["Path"]), isBuiltIn) + Resources::JsonStrToString(data["Right"]));
		cube.push_back(Resources::GetAssetFullPath(Resources::JsonStrToString(data["Path"]), isBuiltIn) + Resources::JsonStrToString(data["Left"]));
		cube.push_back(Resources::GetAssetFullPath(Resources::JsonStrToString(data["Path"]), isBuiltIn) + Resources::JsonStrToString(data["Up"]));
		cube.push_back(Resources::GetAssetFullPath(Resources::JsonStrToString(data["Path"]), isBuiltIn) + Resources::JsonStrToString(data["Down"]));
		cube.push_back(Resources::GetAssetFullPath(Resources::JsonStrToString(data["Path"]), isBuiltIn) + Resources::JsonStrToString(data["Front"]));
		cube.push_back(Resources::GetAssetFullPath(Resources::JsonStrToString(data["Path"]), isBuiltIn) + Resources::JsonStrToString(data["Back"]));
		return cube;
	}

	TextureFullData* Resources::LoadTextureFullData(const string& path, bool isBuiltIn)
	{
		TextureFullData* textureFullData = new TextureFullData();

		textureFullData->path = path;
#ifdef ZX_API_OPENGL
		textureFullData->data = LoadTexture(path.c_str(), &textureFullData->width, &textureFullData->height, &textureFullData->numChannel, 0);
#else
		textureFullData->data = LoadTexture(path.c_str(), &textureFullData->width, &textureFullData->height, &textureFullData->numChannel, STBI_rgb_alpha);
#endif

		if (!textureFullData->data)
			Debug::LogError("Failed to load texture: " + path);

		return textureFullData;
	}

	CubeMapFullData* Resources::LoadCubeMapFullData(const vector<string>& paths, bool isBuiltIn)
	{
		CubeMapFullData* cubeMapFullData = new CubeMapFullData();

		for (size_t i = 0; i < paths.size(); i++)
		{
#ifdef ZX_API_OPENGL
			cubeMapFullData->data[i] = LoadTexture(paths[i].c_str(), &cubeMapFullData->width, &cubeMapFullData->height, &cubeMapFullData->numChannel, 0);
#else
			cubeMapFullData->data[i] = LoadTexture(paths[i].c_str(), &cubeMapFullData->width, &cubeMapFullData->height, &cubeMapFullData->numChannel, STBI_rgb_alpha);
#endif

			if (!cubeMapFullData->data[i])
				Debug::LogError("Failed to load cube map: " + paths[i]);
		}

		return cubeMapFullData;
	}

	void Resources::CheckAsyncLoad()
	{
		for (size_t i = 0; i < mPrefabLoadHandles.size(); i++)
		{
			if (mPrefabLoadHandles[i].future.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
			{
				PrefabStruct* prefab = mPrefabLoadHandles[i].future.get();
				mPrefabLoadHandles[i].callback(prefab);
				// TODO: 检查内存泄漏
				delete prefab;
				mPrefabLoadHandles.erase(mPrefabLoadHandles.begin() + i);
				i--;
			}
		}
		for (size_t i = 0; i < mDiscardedPrefabLoadHandles.size(); i++)
		{
			if (mDiscardedPrefabLoadHandles[i].future.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
			{
				delete mDiscardedPrefabLoadHandles[i].future.get();
				mDiscardedPrefabLoadHandles.erase(mDiscardedPrefabLoadHandles.begin() + i);
				i--;
			}
		}

		for (size_t i = 0; i < mMaterialLoadHandles.size(); i++)
		{
			if (mMaterialLoadHandles[i].future.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
			{
				MaterialStruct* material = mMaterialLoadHandles[i].future.get();
				mMaterialLoadHandles[i].callback(material);
				delete material;
				mMaterialLoadHandles.erase(mMaterialLoadHandles.begin() + i);
				i--;
			}
		}
		for (size_t i = 0; i < mDiscardedMaterialLoadHandles.size(); i++)
		{
			if (mDiscardedMaterialLoadHandles[i].future.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
			{
				delete mDiscardedMaterialLoadHandles[i].future.get();
				mDiscardedMaterialLoadHandles.erase(mDiscardedMaterialLoadHandles.begin() + i);
				i--;
			}
		}

		for (size_t i = 0; i < mModelDataLoadHandles.size(); i++)
		{
			if (mModelDataLoadHandles[i].future.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
			{
				ModelData* modelData = mModelDataLoadHandles[i].future.get();
				mModelDataLoadHandles[i].callback(modelData);
				delete modelData;
				mModelDataLoadHandles.erase(mModelDataLoadHandles.begin() + i);
				i--;
			}
		}
		for (size_t i = 0; i < mDiscardedModelDataLoadHandles.size(); i++)
		{
			if (mDiscardedModelDataLoadHandles[i].future.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
			{
				delete mDiscardedModelDataLoadHandles[i].future.get();
				mDiscardedModelDataLoadHandles.erase(mDiscardedModelDataLoadHandles.begin() + i);
				i--;
			}
		}

#ifdef ZX_EDITOR
		for (size_t i = 0; i < mEditorMaterialLoadHandles.size(); i++)
		{
			if (mEditorMaterialLoadHandles[i].future.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
			{
				MaterialStruct* material = mEditorMaterialLoadHandles[i].future.get();
				mEditorMaterialLoadHandles[i].callback(material);
				delete material;
				mEditorMaterialLoadHandles.erase(mEditorMaterialLoadHandles.begin() + i);
				i--;
			}
		}
		for (size_t i = 0; i < mDiscardedEditorMaterialLoadHandles.size(); i++)
		{
			if (mDiscardedEditorMaterialLoadHandles[i].future.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
			{
				delete mDiscardedEditorMaterialLoadHandles[i].future.get();
				mDiscardedEditorMaterialLoadHandles.erase(mDiscardedEditorMaterialLoadHandles.begin() + i);
				i--;
			}
		}

		for (size_t i = 0; i < mEditorModelDataLoadHandles.size(); i++)
		{
			if (mEditorModelDataLoadHandles[i].future.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
			{
				ModelData* modelData = mEditorModelDataLoadHandles[i].future.get();
				mEditorModelDataLoadHandles[i].callback(modelData);
				delete modelData;
				mEditorModelDataLoadHandles.erase(mEditorModelDataLoadHandles.begin() + i);
				i--;
			}
		}
		for (size_t i = 0; i < mDiscardedEditorModelDataLoadHandles.size(); i++)
		{
			if (mDiscardedEditorModelDataLoadHandles[i].future.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
			{
				delete mDiscardedEditorModelDataLoadHandles[i].future.get();
				mDiscardedEditorModelDataLoadHandles.erase(mDiscardedEditorModelDataLoadHandles.begin() + i);
				i--;
			}
		}
#endif
	}

	void Resources::ClearAsyncLoad()
	{
		for (size_t i = 0; i < mPrefabLoadHandles.size(); i++)
		{
			mDiscardedPrefabLoadHandles.push_back(std::move(mPrefabLoadHandles[i]));
		}
		mPrefabLoadHandles.clear();

		for (size_t i = 0; i < mMaterialLoadHandles.size(); i++)
		{
			mDiscardedMaterialLoadHandles.push_back(std::move(mMaterialLoadHandles[i]));
		}
		mMaterialLoadHandles.clear();

		for (size_t i = 0; i < mModelDataLoadHandles.size(); i++)
		{
			mDiscardedModelDataLoadHandles.push_back(std::move(mModelDataLoadHandles[i]));
		}
		mModelDataLoadHandles.clear();
	}

#ifdef ZX_EDITOR
	void Resources::ClearEditorAsyncLoad()
	{
		for (size_t i = 0; i < mEditorMaterialLoadHandles.size(); i++)
		{
			mDiscardedEditorMaterialLoadHandles.push_back(std::move(mEditorMaterialLoadHandles[i]));
		}
		mEditorMaterialLoadHandles.clear();

		for (size_t i = 0; i < mEditorModelDataLoadHandles.size(); i++)
		{
			mDiscardedEditorModelDataLoadHandles.push_back(std::move(mEditorModelDataLoadHandles[i]));
		}
		mEditorModelDataLoadHandles.clear();
	}
#endif

	void Resources::AsyncLoadPrefab(const string& path, std::function<void(PrefabStruct*)> callback, bool isBuiltIn)
	{
		std::promise<PrefabStruct*> promise;
		std::future<PrefabStruct*> future = promise.get_future();
		std::thread th(DoAsyncLoadPrefab, std::move(promise), path, isBuiltIn);
		th.detach();

		PrefabLoadHandle handle;
		handle.future = std::move(future);
		handle.callback = std::move(callback);
		mPrefabLoadHandles.push_back(std::move(handle));
	}

	void Resources::DoAsyncLoadPrefab(std::promise<PrefabStruct*>&& promise, string path, bool isBuiltIn)
	{
		PrefabStruct* prefab = LoadPrefab(path, isBuiltIn, true);
		promise.set_value(prefab);
	}

	void Resources::AsyncLoadMaterial(const string& path, std::function<void(MaterialStruct*)> callback, bool isBuiltIn, bool isEditor)
	{
		std::promise<MaterialStruct*> promise;
		std::future<MaterialStruct*> future = promise.get_future();
		std::thread th(DoAsyncLoadMaterial, std::move(promise), path, isBuiltIn);
		th.detach();

		MaterialLoadHandle handle;
		handle.future = std::move(future);
		handle.callback = std::move(callback);
#ifdef ZX_EDITOR
		if (isEditor)
			mEditorMaterialLoadHandles.push_back(std::move(handle));
		else
			mMaterialLoadHandles.push_back(std::move(handle));
#else
		mMaterialLoadHandles.push_back(std::move(handle));
#endif
	}

	void Resources::DoAsyncLoadMaterial(std::promise<MaterialStruct*>&& promise, string path, bool isBuiltIn)
	{
		MaterialStruct* material = LoadMaterial(path, isBuiltIn);
		promise.set_value(material);
	}

	void Resources::AsyncLoadModelData(const string& path, std::function<void(ModelData*)> callback, bool isBuiltIn, bool isEditor)
	{
		std::promise<ModelData*> promise;
		std::future<ModelData*> future = promise.get_future();
		std::thread th(DoAsyncLoadModelData, std::move(promise), path, isBuiltIn);
		th.detach();

		ModelDataLoadHandle handle;
		handle.future = std::move(future);
		handle.callback = std::move(callback);
#ifdef ZX_EDITOR
		if (isEditor)
			mEditorModelDataLoadHandles.push_back(std::move(handle));
		else
			mModelDataLoadHandles.push_back(std::move(handle));
#else
		mModelDataLoadHandles.push_back(std::move(handle));
#endif
	}

	void Resources::DoAsyncLoadModelData(std::promise<ModelData*>&& promise, string path, bool isBuiltIn)
	{
		ModelData* modelData = ModelUtil::LoadModel(path, isBuiltIn, true);
		promise.set_value(modelData);
	}
}