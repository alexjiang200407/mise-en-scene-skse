#pragma once
#include "logger.h"

namespace MES
{
	// A physical object within a scene, e.g. a Sweetroll!
	class SceneObj
	{
	public:
		SceneObj() = default;
		SceneObj(RE::TESObjectREFR* ref);
		~SceneObj() = default;

	public:
		RE::TESObjectREFR* GetRef() const;
		void SetRef(RE::TESObjectREFR* rhs);

		bool DeleteRef();

		virtual bool Serialize(SKSE::SerializationInterface* intfc) const;

	private:
		RE::TESObjectREFR* pRef = nullptr;
	};
}