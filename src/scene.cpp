#include "scene.h"
#include "utility.h"
#include "serialization.h"

MES::Scene* MES::Scene::GetSingleton()
{
    static MES::Scene scene;
    return &scene;
}

void MES::Scene::PrintAllObj()
{
    logger::info("Printing All Objects");

    if (objs.size() == 0)
    {
        logger::info("No Objects!");
        return;
    }

    for (uint8_t i = 0; auto& obj : objs)
    {
        logger::info("Object {}: Address: {}", i, fmt::ptr(obj));
        i++;
    }
}

MES::Scene::Scene()
{
    PrintAllObj();
}

MES::Scene::~Scene()
{
    logger::debug("Destructing Scene!");
}

void MES::Scene::StartPositioning()
{
    logger::trace("MES::Scene::StartPositioning");

    RE::UI* ui = RE::UI::GetSingleton();

    // Checks if another menu is open
    for (auto& menu : ui->menuStack)
    {
        if (
            !menu->AlwaysOpen() ||
            menu->Modal() ||
            menu->FreezeFramePause()
        )
        {
            logger::warn("Another menu is open, cannot open MES menu!!!");
            return;
        }
    }

    // Creates object next to player
    RE::TESObjectREFR* ref = CreateObj<RE::TESObjectLIGH>(0x0001d4ec);
    
    if (!ref)
        return;

    MES::Scene::GetSingleton()->isOpen = true;
}

void MES::Scene::StopPositioning()
{
    logger::trace("MES::Scene::StopPositioning");

    RE::PlaySound("VOCShoutDragon01AFus");
    MES::Scene::GetSingleton()->isOpen = false;
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

    uint8_t size = objs.size();

    // First byte always holds amount of objs
    intfc->WriteRecordData(size);

    for (auto& obj : objs)
    {
        if (!obj->Serialize(intfc))
        {
            return;
        };
    }
}

void MES::Scene::GetSceneData(SKSE::SerializationInterface* intfc, uint8_t objCount)
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

    PrintAllObj();
}

void MES::Scene::ClearSceneData()
{
    logger::trace("MES::Scene::ClearSceneData");
    logger::info("Clearing scene data!!");

    PrintAllObj();

    objs.clear();
}

void MES::Scene::PlaceObj()
{
    logger::trace("MES::Scene::PlaceObj");
}