#include "MES.h"
#include "event.h"

void MES::Init() 
{
    MES::SetupLog();
    
    // spdlog::set_level(spdlog::level::err);

    logger::info("MES has initiated!!");
    logger::trace("MES::Init");
}

void MES::RegisterEventHandler()
{
    auto& InputEventProcessor = MES::InputEventProcessor::GetSingleton();

    RE::ScriptEventSourceHolder::GetSingleton()->AddEventSink<RE::TESActivateEvent>(&InputEventProcessor);
    RE::BSInputDeviceManager::GetSingleton()->AddEventSink<RE::InputEvent*>(&InputEventProcessor);
}

void MES::ProcessSysMessages(SKSE::MessagingInterface::Message *msg)
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
        MES::RegisterEventHandler();
    }
    break;
    case SKSE::MessagingInterface::kNewGame:
    {
        logger::info("Started a new game!!");
    }
    break;
    case SKSE::MessagingInterface::kDataLoaded:
    {
        logger::info("The game has loaded all the forms.");
    }
    break;
    case SKSE::MessagingInterface::kSaveGame:
    {
        logger::info("The game has been saved to {}!", static_cast<const char *>(msg->data));
    }
    break;
    }
}


SKSEPluginLoad(const SKSE::LoadInterface *skse) 
{
    SKSE::Init(skse);

    // Initialise MES
    MES::Init();

    // Registers MES's system event listeners
    SKSE::GetMessagingInterface()->RegisterListener(MES::ProcessSysMessages);

    return true;
}


