#pragma once
#include "logger.h"

namespace MES
{
	class SceneObj
	{
	public:
		SceneObj() = default;
		SceneObj(RE::TESObjectREFR* ref);
		~SceneObj() = default;

	public:
		RE::TESObjectREFR* GetRef() const;
		void SetRef(RE::TESObjectREFR* ref);


		virtual bool Serialize(SKSE::SerializationInterface* intfc) const;

	private:
		RE::TESObjectREFR* pRef = nullptr;
	};
}