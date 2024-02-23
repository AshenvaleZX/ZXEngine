#pragma once
#include "Component.h"

namespace ZXEngine
{
	class AudioClip;
	class AudioSource : public Component
	{
		friend class GameObject;
	public:
		static ComponentType GetType();

	public:
		string mName;
		bool mPlayOnAwake = false;

		AudioSource();
		~AudioSource();

		virtual void Awake();
		virtual ComponentType GetInsType();

		void Init(const string& path);
		void Play2D(bool loop = false);
		void Play3D(bool loop = false);
		void Play3D(const Vector3& position, bool loop = false);
		bool GetIs3D();
		void Stop();
		void SetPause(bool pause);
		bool GetPause();
		void SetLoop(bool loop);
		bool GetLoop();
		void SetVolume(float volume);
		float GetVolume();
		void SetPosition(const Vector3& position);
		void SetMinDistance(float distance);
		float GetMinDistance();
		void SetMaxDistance(float distance);
		float GetMaxDistance();

	private:
		bool mIs3D = false;
		bool mIsLoop = false;
		bool mIsPause = false;
		float mVolume = 1.0f;
		AudioClip* mAudioClip = nullptr;
	};
}