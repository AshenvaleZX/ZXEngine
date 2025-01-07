#pragma once
#include "../pubh.h"
#include "ParticleSystemDefinition.h"

namespace ZXEngine
{
	class Camera;
	class Material;
	class ParticleRenderer
	{
		friend class EditorInspectorPanel;
	public:
		string mTexturePath;

		ParticleRenderer() = default;
		~ParticleRenderer();

		void Render(Camera* camera, const vector<Particle>& particles);
		void SetTexture(const string& path);

		void Init(const ParticleSystemState& state);

	private:
		vector<ParticleInstanceData> mInstanceData;
		uint32_t mInstanceBuffer = UINT32_MAX;
		uint32_t mInstanceDataSize = 0; // 以Vector4为单位
		uint32_t mInstanceActiveNum = 0;

		uint32_t mVAO = UINT32_MAX;
		uint32_t mTextureID = UINT32_MAX;
		Material* mMaterial = nullptr;

		void UpdateInstanceData(const Vector3& camPos, const vector<Particle>& particles);
	};

	ZXRef_StaticReflection
	(
		ParticleRenderer,
		ZXRef_Fields
		(
			ZXRef_Field(&ParticleRenderer::mTexturePath)
		)
	)
}