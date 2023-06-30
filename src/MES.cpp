#include "MES.h"
#include <nlohmann/json.hpp>
#include <string>

void MES::Init() 
{
    MES::SetupLog();
    logger::trace("MES::Init");

    auto* serialisation = SKSE::GetSerializationInterface();

    // Register serialisation
    serialisation->SetUniqueID(Serialization::kMESID);
    serialisation->SetSaveCallback(Serialization::SaveCallback);
    serialisation->SetLoadCallback(Serialization::LoadCallback);
    serialisation->SetRevertCallback(Serialization::RevertCallback);


    // Registers MES's system event listeners
    SKSE::GetMessagingInterface()->RegisterListener(MES::ProcessSysMessages);


    logger::info("MES has initiated!!");
}

void MES::RegisterEventHandler()
{
    EventProcessor::GetSingleton()->Register();
    UIManager::GetSingleton()->Register();
}

void MES::UnregisterEventHandler()
{
    MES::EventProcessor::GetSingleton()->Unregister();
}

void MES::ProcessSysMessages(SKSE::MessagingInterface::Message* msg)
{
    logger::trace("MES::ProcessSysMessages");

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
        MES::Scene::GetSingleton()->GetProps().clear();

        logger::info("Started a new game!!");
    }
    break;
    case SKSE::MessagingInterface::kDataLoaded:
    {
        logger::info("The game has loaded all the forms.");

        MES::RegisterEventHandler();
        MES::GetSavedBaseObjIds();
    }
    break;
    case SKSE::MessagingInterface::kSaveGame:
    {
        logger::info("The game has been saved to {}!", static_cast<const char*>(msg->data));
    }
    break;
    }
}

void MES::GetSavedBaseObjIds()
{
    logger::trace("MES::GetSavedBaseObjIds");
    using json = nlohmann::json;

    std::ifstream file{ ".\\Data\\SKSE\\Plugins\\MiseEnScene.json" };
    uint32_t      modIndex = GetPluginCompileIndex();

    if (!file.is_open())
    {
        logger::error("MiseEnScene.json not found!");
        return;
    }

    if (modIndex == 0)
    {
        logger::error("{} not found!", MES::Scene::GetFileName());
        return;
    }

    logger::info("Found MiseEnScene.json");

    json        data  = json::parse(file);
    MES::Scene* scene = MES::Scene::GetSingleton();

    // Gets saved object ids from json file
    for (int i = 0; auto& objType : data)
    {
        // Checks if objType object contains right fields
        if (
            !objType.contains("objs") || 
            !objType.contains("objType") || 
            !objType["objType"].is_string() ||
            !objType["objs"].is_array()
        )
            continue;

        scene->GetBoundObjs().push_back({ std::string(objType["objType"]).c_str() });
        scene->GetBoundObjs()[i].objs.reserve(objType["objs"].size());

        for (auto& obj : objType["objs"])
        {

            if (
                !obj.is_object() ||
                !obj.contains("formId") ||
                !obj.contains("buttonText")
            )
                continue;


            // Converts hexadecimal string to integer
            RE::FormID id = stoi(static_cast<std::string>(obj["formId"]), 0, 16);

            // Adds the mod compile index to the start of the id
            id += (modIndex << (4 * 6));

            logger::info("Adding Base Object of type: {} with ID {:x}. i is {}", objType["objType"], id, i);

            scene->GetBoundObjs()[i].objs.push_back({ id, std::string(obj["buttonText"]).c_str() });
        }
        i++;
    }
}

uint8_t MES::GetPluginCompileIndex()
{
    RE::TESDataHandler* dhnd = RE::TESDataHandler::GetSingleton();

    for (const auto& file : dhnd->files)
    {
        if (file->GetFilename() == MES::Scene::GetFileName())
        {
            logger::info("ESP load order {:x}", file->compileIndex);
            return file->compileIndex;
        }
    }

    return 0;
}


SKSEPluginLoad(const SKSE::LoadInterface* skse) 
{
    SKSE::Init(skse);

    // Initialise MES
    MES::Init();

    return true;
}


