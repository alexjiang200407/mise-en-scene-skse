#pragma once

namespace MES
{
	class SceneObj
	{
	public:
		SceneObj() = default;
		SceneObj(RE::TESObjectREFR* ref);
		~SceneObj() = default;

		// SceneObj(const SceneObj&) = delete;
		// SceneObj(SceneObj&&) = delete;
		// SceneObj& operator=(const SceneObj&) = delete;
		// SceneObj& operator=(SceneObj&&) = delete;

	public:
		RE::TESObjectREFR* GetRef() const;

	private:
		RE::TESObjectREFR* pRef;
	};
}