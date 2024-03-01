#pragma once
#include "../pubh.h"
#include "Component.h"

namespace ZXEngine
{
	class GameLogic : public Component
	{
	public:
		static ComponentType GetType();

	public:
		string luaName;
		string luaFullPath;

		GameLogic();
		~GameLogic();

		virtual void Awake();
		virtual ComponentType GetInsType();

		void Start();
		void Update();
		void FixedUpdate();
		void CallLuaFunction(const char* func);

	private:
		int luaID = 0;
		bool firstCall = true;
	};
}