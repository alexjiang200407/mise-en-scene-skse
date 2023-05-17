#include "MES.h"


void MES::Init() 
{
    MES::SetupLog();
    
    // spdlog::set_level(spdlog::level::err);

    logger::info("MES has initiated!!");
    logger::trace("MES::Init");
    auto* serialisation = SKSE::GetSerializationInterface();

    // Register serialisation
    serialisation->SetUniqueID(Serialization::kMESID);
    serialisation->SetSaveCallback(Serialization::SaveCallback);
    serialisation->SetLoadCallback(Serialization::LoadCallback);
    serialisation->SetRevertCallback(Serialization::RevertCallback);
}

void MES::RegisterEventHandler()
{
    auto& EventProcessor = MES::EventProcessor::GetSingleton();

    RE::ScriptEventSourceHolder::GetSingleton()->AddEventSink<RE::TESActivateEvent>(&EventProcessor);
    RE::BSInputDeviceManager::GetSingleton()->AddEventSink<RE::InputEvent*>(&EventProcessor);
}

void MES::UnregisterEventHandler()
{
    auto& EventProcessor = MES::EventProcessor::GetSingleton();

    RE::ScriptEventSourceHolder::GetSingleton()->RemoveEventSink<RE::TESActivateEvent>(&EventProcessor);
    RE::BSInputDeviceManager::GetSingleton()->RemoveEventSink(&EventProcessor);

}

void MES::ProcessSysMessages(SKSE::MessagingInterface::Message* msg)
{
    logger::trace("MES::ProcessSysMessages");
    logger::info("Message Type: {}", msg->type);

    switch (msg->type)
    {
    case SKSE::MessagingInterface::kPreLoadGame:
    {
         logger::info("About to blow a load.");
    }
    break;
    case SKSE::MessagingInterface::kPostLoadGame:
    {
        logger::info("AHHHHH!!! Just blew a load. Load OK?!! {}", static_cast<bool>(msg->data));
    }
    break;
    case SKSE::MessagingInterface::kNewGame:
    {
        // Clears previous scene data
        MES::Scene::GetSingleton()->GetObjs().clear();

        logger::info("Started a new game!!");
    }
    break;
    case SKSE::MessagingInterface::kDataLoaded:
    {
        logger::info("The game has loaded all the forms.");

        // TODO Make sure the event handlers only fire when ingame not when the opening menu is open
        MES::RegisterEventHandler();

    }
    break;
    case SKSE::MessagingInterface::kSaveGame:
    {
        // Saves scene data to disk
        // MES::Scene::GetSingleton()->SaveSceneData();

        logger::info("The game has been saved to {}!", static_cast<const char*>(msg->data));
    }
    break;
    }
}


SKSEPluginLoad(const SKSE::LoadInterface* skse) 
{
    SKSE::Init(skse);

    // Initialise MES
    MES::Init();

    // Registers MES's system event listeners
    SKSE::GetMessagingInterface()->RegisterListener(MES::ProcessSysMessages);

    return true;
}


