#include "scene.h"
#include "utility.h"
#include "serialization.h"
#include "UIManager.h"

MES::Scene* MES::Scene::GetSingleton()
{
    static MES::Scene scene;
    return &scene;
}

std::vector<std::unique_ptr<MES::SceneObj>>& MES::Scene::GetObjs()
{
    return objs;
}

void MES::Scene::PrintAllObj()
{
    logger::info("Printing All Objects");

    if (objs.size() == 0)
    {
        logger::info("No Objects!");
        return;
    }

    // Prints relevant debug information
    for (uint8_t i = 0; auto& obj : objs)
    {
        logger::info(
            "Object: {}, Address: {}, ID: {:x}, Ref Count: {}", 
            i, fmt::ptr(obj), obj->GetRef()->GetFormID(),
            obj->GetRef()->QRefCount()
        );

        i++;
    }
}

MES::Scene::Scene()
{
    objs.reserve(static_cast<uint16_t>(maxObj));
}

MES::Scene::~Scene()
{
    logger::debug("Destructing Scene!");
}

void MES::Scene::StartPositioning()
{
    logger::trace("MES::Scene::StartPositioning");

    if (!UIManager::GetSingleton()->OpenMenu())
        return;

    // TODO make it so actors will have AI toggled off

    // Creates object next to player
    // dynamic light flicker 0x18fed
    // torch 0x36343
    // Actor Vex 1cd90

     RE::TESObjectREFR* ref = CreateObj(0x1cd90);

    if (!ref)
        return;

    // SKSE::GetCrosshairRefEventSource()->AddEventSink();

    MES::UIManager::GetSingleton()->isOpen = true;
}

void MES::Scene::StopPositioning()
{
    logger::trace("MES::Scene::StopPositioning");

    UIManager::GetSingleton()->CloseMenu();
}

void MES::Scene::SaveSceneData(SKSE::SerializationInterface* intfc) const
{
    logger::trace("MES::Scene::SaveSceneData");
    logger::info("Saving scene data!");

    if (!intfc->OpenRecord(Serialization::kMESID, Serialization::kSerializationVersion))
    {
        logger::error("Failed to open Scene Data record!");
        return;
    }

    uint8_t size = static_cast<uint8_t>(objs.size());

    logger::info("Saving size: {} to file.", size);

    // First byte always holds amount of objs
    intfc->WriteRecordData(size);

    if (size == 0)
    {
        logger::info("No objects to save!");
        return;
    }

    for (auto& obj : objs)
    {
        if (!obj->Serialize(intfc))
        {
            return;
        };
    }
}

void MES::Scene::SerializeScene(SKSE::SerializationInterface* intfc, uint8_t objCount)
{
    logger::info("Getting saved scene data!");
    
    
    for (uint8_t i = 0u; i < objCount; i++)
    {
        MES::SceneObj obj;
        RE::FormID fid;
        
        intfc->ReadRecordData(fid);

        RE::TESForm* tesform = RE::TESForm::LookupByID(fid);

        if (tesform != nullptr)
        {
            logger::info(
                "Unserialising SceneObj {} with formid: {:x}",
                i, fid
            );

            obj.SetRef(tesform->AsReference());
        }
        else
        {
            logger::error("Formid {:x} not found!!!", fid);
            return;
        }

        objs.push_back(std::make_unique<SceneObj>(obj));
    }
}

void MES::Scene::ClearScene()
{
    logger::trace("MES::Scene::ClearScene");
    logger::info("Clearing scene data!!");

    // Remove all in game references
    for (auto& obj : objs)
    {
        // Deletes the reference to the object
        obj->DeleteRef();
    }

    objs.clear();
}

RE::TESObjectREFR* MES::Scene::CreateObj(RE::TESBoundObject* baseObj)
{
    RE::TESObjectREFR* playerRef = RE::PlayerCharacter::GetSingleton()->AsReference();

    if (!baseObj)
    {
        logger::warn("base object was not found!");
        return nullptr;
    }

    if (!(playerRef))
    {
        logger::error("Player reference not found");
        return nullptr;
    }

    // If the obj count surpasses maximum size
    if (objs.size() >= maxObj)
    {
        char buffer[256];
        snprintf(
            buffer, 256, "Cannot have more than %d scene objects!", maxObj
        );

        RE::DebugNotification(buffer, "UIMenuOKSD");
        return nullptr;
    }

    // Creates new object reference and places it at player
    RE::TESObjectREFR* newObjRef = playerRef->PlaceObjectAtMe(baseObj, true).get();

    objs.push_back(std::make_unique<SceneObj>(newObjRef));

    // Handles based on the baseObj form type
    switch (baseObj->GetFormType())
    {
    // If NPC toggles its AI
    case RE::FormType::NPC:
    {
        newObjRef->As<RE::Actor>()->EnableAI(false);
    }
    break;
    }

    logger::info(
        "Created Object! Type: {}, Base ID: {:x}, Ref ID: {:x},",
        RE::FormTypeToString(baseObj->GetFormType()), baseObj->GetFormID(),
        newObjRef->GetFormID()
    );

    return newObjRef;
}

RE::TESObjectREFR* MES::Scene::CreateObj(RE::FormID baseId)
{
    RE::TESForm* obj = RE::TESForm::LookupByID(baseId);

    if (!obj)
    {
        logger::warn("baseID: {:x} doesn't exist!", baseId);
        return nullptr;
    }

    return CreateObj(obj->As<RE::TESBoundObject>());
}

void MES::Scene::PlaceObj()
{
    logger::trace("MES::Scene::PlaceObj");
}