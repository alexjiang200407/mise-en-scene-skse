#pragma once
#include "logger.h"
#include "obj.h"


namespace MES
{
	class Scene
		:
		public RE::BSTEventSink<RE::InputEvent*>
	{
	public:
		Scene();
		~Scene();
		Scene(const Scene&) = delete;
		Scene(Scene&&) = delete;
		Scene& operator=(const Scene&) = delete;
		Scene& operator=(Scene&&) = delete;

	public:
		// If player is adding new object to the scene
		void StartPositioning();

		// If player is modifying existing object in the scene
		void StartPositioning(uint8_t index);

		// Positions a scene object in game world, callback called after user finishes positioning
		void StartPositioning(MES::SceneObj* sceneObj);
		
		void StopPositioning();
		void PlaceObj();


		// Serializes the scene's data to file
		void SaveSceneData(SKSE::SerializationInterface* intfc) const;

		// Unserializes Scene Data from file
		void SerializeScene(SKSE::SerializationInterface* intfc, uint8_t objCount);

		// Clears the scene's data
		void ClearScene();

		// Creates an object based on base object
		RE::TESObjectREFR* CreateObj(RE::TESBoundObject* baseObj);

		// Creates an object based on id and returns the reference to the object
		RE::TESObjectREFR* CreateObj(RE::FormID baseId);

		static MES::Scene* GetSingleton();

		std::vector<std::unique_ptr<SceneObj>>& GetObjs() { return objs; };

		std::unique_ptr<MES::SceneObj>& GetPositioned() { return positionedObj; };

		// Processes the cursor move event
		RE::BSEventNotifyControl ProcessEvent(
			RE::InputEvent* const* event, 
			RE::BSTEventSource<RE::InputEvent*>*
		) override;
		
	// Debug
	public:
		void PrintAllObj();

	private:
		static constexpr uint8_t maxObj = 255;
		std::vector<std::unique_ptr<SceneObj>> objs;
		bool newObj = false;
		std::unique_ptr<SceneObj> positionedObj;
	};
}
