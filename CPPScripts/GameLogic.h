#pragma once
#include "pubh.h"
#include "Component.h"

namespace ZXEngine
{
	class GameLogic : public Component
	{
	public:
		int luaID = 0;
		string luaName;

		GameLogic();
		~GameLogic();

		static ComponentType GetType();

		void Start();
		void Update();
		void CallLuaFunction(const char* func);

	private:
		bool firstCall = true;
	};
}