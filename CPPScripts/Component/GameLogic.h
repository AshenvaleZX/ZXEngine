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
		int luaID = 0;
		string Lua;

		unordered_map<string, bool> mBoolVariables;
		unordered_map<string, float> mFloatVariables;
		unordered_map<string, int32_t> mIntVariables;
		unordered_map<string, string> mStringVariables;

		GameLogic();
		~GameLogic();

		virtual void Awake();
		virtual ComponentType GetInsType();

		const string& GetLuaName();
		const string& GetLuaFullPath();

		void Start();
		void Update();
		void FixedUpdate();
		void CallLuaFunction(const char* func);
		void SetLuaVariable(const string& name, bool value);
		void SetLuaVariable(const string& name, float value);
		void SetLuaVariable(const string& name, int32_t value);
		void SetLuaVariable(const string& name, const string& value);

	private:
		bool firstCall = true;
		string mLuaName;
		string mLuaFullPath;
	};

	StaticReflection
	(
		GameLogic,
		Fields
		(
			Field(&GameLogic::Lua),
			Field(&GameLogic::mBoolVariables),
			Field(&GameLogic::mFloatVariables),
			Field(&GameLogic::mIntVariables),
			Field(&GameLogic::mStringVariables)
		)
	)
}