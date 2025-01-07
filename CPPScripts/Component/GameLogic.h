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
		void SetLuaVariable(const string& name, bool value, bool sync = true);
		void SetLuaVariable(const string& name, float value, bool sync = true);
		void SetLuaVariable(const string& name, int32_t value, bool sync = true);
		void SetLuaVariable(const string& name, const string& value, bool sync = true);

	private:
		bool firstCall = true;
		string mLuaName;
		string mLuaFullPath;
	};

	ZXRef_StaticReflection
	(
		GameLogic,
		ZXRef_BaseType(Component)
		ZXRef_Fields
		(
			ZXRef_Field(&GameLogic::Lua),
			ZXRef_Field(&GameLogic::mBoolVariables),
			ZXRef_Field(&GameLogic::mFloatVariables),
			ZXRef_Field(&GameLogic::mIntVariables),
			ZXRef_Field(&GameLogic::mStringVariables)
		)
	)
}