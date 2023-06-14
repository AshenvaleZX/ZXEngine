#include "Material.h"
#include "ZShader.h"
#include "Texture.h"
#include "RenderAPI.h"
#include "GlobalData.h"
#include "MaterialData.h"
#include "RenderEngineProperties.h"

namespace ZXEngine
{
	Material::Material(MaterialStruct* matStruct)
	{
		name = matStruct->name;
		path = matStruct->path;
		isShareShader = false;
		type = matStruct->type;
		data = new MaterialData(type);

		CopyMaterialStructToMaterialData(matStruct, data);

		if (type == MaterialType::Rasterization)
		{
			shader = new Shader(matStruct->shaderPath, FrameBufferType::Normal);
			renderQueue = (int)shader->reference->shaderInfo.stateSet.renderQueue;
			RenderAPI::GetInstance()->SetUpMaterial(shader->reference, data);
		}
		else if (type == MaterialType::RayTracing)
		{
			// ��׷������ʱ����Ҫ���ֶ��У���һ��Ĭ�ϵ�Opaque����
			renderQueue = (int)RenderQueueType::Opaque;
			RenderAPI::GetInstance()->SetUpRayTracingMaterialData(this);
		}
	}

	Material::Material(Shader* shader)
	{
		// ����Ҫע��һ�����⣬�������������캯��ʱ����ֱ�Ӱ�new Shaderд�ڲ�����ģ���:
		// new Material(new Shader(...))
		// ��ô����һ�����⣬new��ʱ��û�б������ã����ջ���һ��delete������referenceCount���ٴ���1�����shader��Զ���ᱻ��������
		isShareShader = true;
		shader->reference->referenceCount++;
		this->shader = shader;
		renderQueue = (int)shader->reference->shaderInfo.stateSet.renderQueue;
		// ͨ��������캯����ʼ���Ĳ��ʣ�Ĭ��Ϊ��դ����Ⱦ���ߵĲ���
		type = MaterialType::Rasterization;
		data = new MaterialData(type);
		RenderAPI::GetInstance()->SetUpMaterial(this->shader->reference, data);
	}

	Material::~Material()
	{
		delete data;

		if (isShareShader)
			shader->reference->referenceCount--;
		else
			delete shader;
	}

	void Material::Use()
	{
		data->Use();
		shader->Use();
	}

	int Material::GetRenderQueue()
	{
		return renderQueue;
	}

	void Material::SetEngineProperties()
	{
		for (auto& property : shader->reference->shaderInfo.vertProperties.baseProperties)
			SetEngineProperty(property.name, property.type);
		for (auto& property : shader->reference->shaderInfo.vertProperties.textureProperties)
			SetEngineProperty(property.name, property.type);
		for (auto& property : shader->reference->shaderInfo.fragProperties.baseProperties)
			SetEngineProperty(property.name, property.type);
		for (auto& property : shader->reference->shaderInfo.fragProperties.textureProperties)
			SetEngineProperty(property.name, property.type);
	}

	void Material::SetEngineProperty(const string& name, ShaderPropertyType type)
	{
		auto engineProperties = RenderEngineProperties::GetInstance();

		if (type == ShaderPropertyType::ENGINE_MODEL)
			SetMatrix(name, engineProperties->matM);
		else if (type == ShaderPropertyType::ENGINE_VIEW)
			SetMatrix(name, engineProperties->matV);
		else if (type == ShaderPropertyType::ENGINE_PROJECTION)
			SetMatrix(name, engineProperties->matP);
		else if (type == ShaderPropertyType::ENGINE_CAMERA_POS)
			SetVector(name, engineProperties->camPos);
		else if (type == ShaderPropertyType::ENGINE_LIGHT_POS)
			SetVector(name, engineProperties->lightPos);
		else if (type == ShaderPropertyType::ENGINE_LIGHT_DIR)
			SetVector(name, engineProperties->lightDir);
		else if (type == ShaderPropertyType::ENGINE_LIGHT_COLOR)
			SetVector(name, engineProperties->lightColor);
		else if (type == ShaderPropertyType::ENGINE_LIGHT_INTENSITY)
			SetScalar(name, engineProperties->lightIntensity);
		else if (type == ShaderPropertyType::ENGINE_FAR_PLANE)
			SetScalar(name, GlobalData::shadowCubeMapFarPlane);
		else if (type == ShaderPropertyType::ENGINE_MODEL_INV)
			SetMatrix(name, engineProperties->matM_Inv);
		else if (type == ShaderPropertyType::ENGINE_DEPTH_CUBE_MAP)
		{
			// ������SetMaterialProperties�����������ĳ�ʼtextureIdx��Ȼ��++
			SetCubeMap(name, engineProperties->shadowCubeMap, textureIdx, false, engineProperties->isShadowCubeMapBuffer);
			textureIdx++;
		}
	}

	void Material::SetMaterialProperties()
	{
		uint32_t textureNum = static_cast<uint32_t>(data->textures.size());

		for (uint32_t i = 0; i < textureNum; i++)
		{
			if (data->textures[i].second->type == TextureType::ZX_2D)
				SetTexture(data->textures[i].first, data->textures[i].second->GetID(), i);
			else if (data->textures[i].second->type == TextureType::ZX_Cube)
				SetCubeMap(data->textures[i].first, data->textures[i].second->GetID(), i);
		}

		textureIdx = textureNum;
	}

	void Material::SetMaterialProperty(const string& name, ShaderPropertyType type)
	{

	}

	void Material::SetScalar(string name, bool value, bool allBuffer)
	{
		RenderAPI::GetInstance()->SetShaderScalar(this, name, value, allBuffer);
	}
	void Material::SetScalar(string name, int value, bool allBuffer)
	{
		RenderAPI::GetInstance()->SetShaderScalar(this, name, value, allBuffer);
	}
	void Material::SetScalar(string name, float value, bool allBuffer)
	{
		RenderAPI::GetInstance()->SetShaderScalar(this, name, value, allBuffer);
	}
	void Material::SetVector(string name, Vector2 value, bool allBuffer)
	{
		RenderAPI::GetInstance()->SetShaderVector(this, name, value, allBuffer);
	}
	void Material::SetVector(string name, Vector2 value, uint32_t idx, bool allBuffer)
	{
		RenderAPI::GetInstance()->SetShaderVector(this, name, value, idx, allBuffer);
	}
	void Material::SetVector(string name, Vector3 value, bool allBuffer)
	{
		RenderAPI::GetInstance()->SetShaderVector(this, name, value, allBuffer);
	}
	void Material::SetVector(string name, Vector3 value, uint32_t idx, bool allBuffer)
	{
		RenderAPI::GetInstance()->SetShaderVector(this, name, value, idx, allBuffer);
	}
	void Material::SetVector(string name, Vector4 value, bool allBuffer)
	{
		RenderAPI::GetInstance()->SetShaderVector(this, name, value, allBuffer);
	}
	void Material::SetVector(string name, Vector4 value, uint32_t idx, bool allBuffer)
	{
		RenderAPI::GetInstance()->SetShaderVector(this, name, value, idx, allBuffer);
	}
	void Material::SetMatrix(string name, Matrix3 value, bool allBuffer)
	{
		RenderAPI::GetInstance()->SetShaderMatrix(this, name, value, allBuffer);
	}
	void Material::SetMatrix(string name, Matrix3 value, uint32_t idx, bool allBuffer)
	{
		RenderAPI::GetInstance()->SetShaderMatrix(this, name, value, idx, allBuffer);
	}
	void Material::SetMatrix(string name, Matrix4 value, bool allBuffer)
	{
		RenderAPI::GetInstance()->SetShaderMatrix(this, name, value, allBuffer);
	}
	void Material::SetMatrix(string name, Matrix4 value, uint32_t idx, bool allBuffer)
	{
		RenderAPI::GetInstance()->SetShaderMatrix(this, name, value, idx, allBuffer);
	}
	void Material::SetTexture(string name, uint32_t ID, uint32_t idx, bool allBuffer, bool isBuffer)
	{
		RenderAPI::GetInstance()->SetShaderTexture(this, name, ID, idx, allBuffer, isBuffer);
	}
	void Material::SetCubeMap(string name, uint32_t ID, uint32_t idx, bool allBuffer, bool isBuffer)
	{
		RenderAPI::GetInstance()->SetShaderCubeMap(this, name, ID, idx, allBuffer, isBuffer);
	}

	void Material::CopyMaterialStructToMaterialData(MaterialStruct* matStruct, MaterialData* data)
	{
		data->floatDatas = matStruct->floatDatas;
		data->uintDatas = matStruct->uintDatas;
		data->vec2Datas = matStruct->vec2Datas;
		data->vec3Datas = matStruct->vec3Datas;
		data->vec4Datas = matStruct->vec4Datas;

		for (auto textureStruct : matStruct->textures)
		{
			Texture* texture = new Texture(textureStruct->path.c_str());
			data->textures.push_back(make_pair(textureStruct->uniformName, texture));
		}

		for (auto cubeMapStruct : matStruct->cubeMaps)
		{
			Texture* texture = new Texture(cubeMapStruct->paths);
			data->textures.push_back(make_pair(cubeMapStruct->uniformName, texture));
		}
	}
}