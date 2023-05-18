#pragma once
#include "logger.h"
#include <list>
#include "obj.h"
#include "UI.h"

namespace MES
{
	class Scene
	{
	public:
		Scene();

		~Scene();
		Scene(const Scene&) = delete;
		Scene(Scene&&) = delete;
		Scene& operator=(const Scene&) = delete;
		Scene& operator=(Scene&&) = delete;

	public:
		void StartPositioning();
		void StopPositioning();

		// Serializes the scene's data to file
		void SaveSceneData(SKSE::SerializationInterface* intfc) const;

		// Unserializes Scene Data from file
		void SerializeScene(SKSE::SerializationInterface* intfc, uint8_t objCount);

		// Clears the scene's data
		void ClearScene();

		RE::TESObjectREFR* CreateObj(RE::TESBoundObject* baseObj);

		// Creates an object based on id and returns the reference to the object
		RE::TESObjectREFR* CreateObj(RE::FormID baseId);
		
		void PlaceObj();

		static MES::Scene* GetSingleton();

		std::list<std::unique_ptr<SceneObj>>& GetObjs();


	// Debug
	public:
		void PrintAllObj();

	private:
		static constexpr uint8_t maxObj = 255;
		static constexpr uint8_t maxLights = 5;
		std::list<std::unique_ptr<SceneObj>> objs;
	};
}
