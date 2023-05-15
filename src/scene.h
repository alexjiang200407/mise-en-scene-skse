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
		void GetSceneData(SKSE::SerializationInterface* intfc, uint8_t objCount);

		// Clears the scene's data
		void ClearSceneData();

		// Creates an object and returns the reference to the object
		template <typename T>
		RE::TESObjectREFR* CreateObj(uint32_t baseId)
		{
			T* obj = RE::TESForm::LookupByID(baseId)->As<T>();
			RE::TESObjectREFR* playerRef = RE::PlayerCharacter::GetSingleton()->AsReference();
			RE::NiPoint3 playerPos = RE::PlayerCharacter::GetSingleton()->GetPosition();

			if (!obj)
			{
				logger::error("Object formid doesn't fucking exist");
				return nullptr;
			}

			int8_t type = obj->formType.underlying();

			if (!(obj->IsInitialized()))
				obj->InitItem();

			logger::info("Object Type: {}", type);
			logger::info("Object Name: {}", obj->GetFormEditorID());
			logger::info("Object Ref Count: {}", obj->GetRefCount());


			if (!(playerRef))
			{
				logger::error("Player reference not found");
				return nullptr;
			}

			// Creates new object reference
			RE::TESObjectREFR* newObjRef = playerRef->PlaceObjectAtMe(obj, true).get();

			// TODO put this in PlaceObj()			
			if (objs.size() >= maxObj)
				objs.pop_front();
				
			objs.push_back(std::make_unique<SceneObj>(SceneObj(newObjRef)));

			RE::PlaySound("VOCShoutDragon01AFus");

			return newObjRef;
		};
		
		void PlaceObj();

		static MES::Scene* GetSingleton();

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
