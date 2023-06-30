#pragma once
#include "logger.h"

namespace MES
{
	// A physical object within a scene, e.g. a Sweetroll!
	class Prop
	{
	public:
		Prop() = default;
		Prop(RE::TESObjectREFR* ref);
		// Prop(const Prop&) = default;

		~Prop() = default;

	public:
		RE::TESObjectREFR* GetRef() const;
		void SetRef(RE::TESObjectREFR* rhs);

		void DeleteRef();
		void MoveToLooking();

		virtual bool Serialize(SKSE::SerializationInterface* intfc) const;

	private:
		RE::TESObjectREFR* pRef = nullptr;
	};
}