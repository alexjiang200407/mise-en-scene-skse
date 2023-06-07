#pragma once
#include "logger.h"
#include "prop.h"


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
	
		void StopPositioning();
		void PlaceProp();

		// Serializes the scene's data to file
		void SerializeScene(SKSE::SerializationInterface* intfc) const;

		// Unserializes Scene Data from file
		void UnserializeScene(SKSE::SerializationInterface* intfc, uint8_t objCount);

		// Clears the scene's data
		void ClearScene();

		// Creates an object based on base object
		RE::TESObjectREFR* CreateProp(RE::TESBoundObject* baseObj);

		// Creates an object based on id and returns the reference to the object
		RE::TESObjectREFR* CreateProp(RE::FormID baseId);

	// Getters
	public:
		static MES::Scene*                  GetSingleton();
		std::vector<std::unique_ptr<Prop>>& GetProps() { return props; };
		std::vector<RE::FormID>&            GetBoundObjs() { return baseObjIds; };
		std::unique_ptr<MES::Prop>&         GetPositioned() { return positionedProp; };
		constexpr static std::string_view   GetFileName() { return fileName; };


		// Processes the cursor move event
		RE::BSEventNotifyControl ProcessEvent(
			RE::InputEvent* const* event, 
			RE::BSTEventSource<RE::InputEvent*>*
		) override;
		


	// Debug
	public:
		void PrintAllProp();

	public:
		// If the user is placing a new physical prop in the scene, not just readjusting old one
		bool                               newProp = false;

	private:
		static constexpr uint8_t           maxProps = 255;

		// All the physical props of the scene, lighting, chairs, etc
		std::vector<std::unique_ptr<Prop>> props;

		// The currently positioned prop
		std::unique_ptr<Prop>              positionedProp;

		// Name of the file
		static constexpr std::string_view  fileName{ "MiseEnScene.esp" };
		
		// IDs of the base objects / bound objects which can be placed by the user
		std::vector<RE::FormID>            baseObjIds;
	};
}
