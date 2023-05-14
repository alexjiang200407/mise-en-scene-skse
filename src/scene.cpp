#include "scene.h"
#include "utility.h"

MES::Scene &MES::Scene::GetSingleton()
{
    static MES::Scene scene;
    return scene;
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

MES::Scene::~Scene()
{
    logger::debug("Destructing Scene!");
}

void MES::Scene::StartPositioning()
{
    logger::trace("MES::Scene::StartPositioning");

    RE::UI* ui = RE::UI::GetSingleton();
    auto* dataHandler = RE::TESDataHandler::GetSingleton();

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

    MES::Scene::GetSingleton().isOpen = true;
}

void MES::Scene::StopPositioning()
{
    logger::trace("MES::Scene::StopPositioning");

    RE::PlaySound("VOCShoutDragon01AFus");
    MES::Scene::GetSingleton().isOpen = false;
}

void MES::Scene::SaveSceneData() const
{
    logger::trace("MES::Scene::SaveSceneData");

    logger::info("Saving scene data!");
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