#pragma once
#include "logger.h"
#include <list>
#include "obj.h"

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
		template <typename T>
		RE::TESObjectREFR* CreateObj(uint32_t baseId)
		{
			T* obj = RE::TESForm::LookupByID(baseId)->As<T>();

			if (!obj)
			{
				logger::error("Object doesn't fucking exist!");
				return nullptr;
			}

			if (!(obj->IsBoundObject()))
			{
				logger::error("Object with id {} is not a base object!", baseId);
				return nullptr;
			};
			
			return CreateObj(obj->As<RE::TESBoundObject>());
		};
		
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

	public:
		// If the user is currently positioning
		bool isOpen = false;
	};
}
