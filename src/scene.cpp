#include "scene.h"
#include "utility.h"

MES::Scene &MES::Scene::GetSingleton()
{
    static MES::Scene scene;
    return scene;
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

    uint32_t baseId = 0x0001d4ec;

    RE::TESObjectLIGH* obj = RE::TESForm::LookupByID(baseId)->As<RE::TESObjectLIGH>();
    RE::TESObjectREFR* playerRef = RE::PlayerCharacter::GetSingleton()->AsReference();
    RE::NiPoint3 playerPos = RE::PlayerCharacter::GetSingleton()->GetPosition();

    if (!obj)
    {
        logger::error("Object formid doesn't fucking exist");
        return;
    }

    int8_t type = obj->formType.underlying();


    logger::info("Object initialised? {}", obj->IsInitialized());

    if (!(obj->IsInitialized()))
        obj->InitItem();

    logger::info("Object Type: {}", type);
    logger::info("Object Name: {}", obj->GetFormEditorID());
    logger::info("Object Ref Count: {}", obj->GetRefCount());
    logger::info("Has current? {}", obj->QHasCurrents());
    

    if (!(playerRef))
    {
        logger::error("Player reference not found");
        return;
    }

    playerRef->PlaceObjectAtMe(obj, true);


    RE::PlaySound("VOCShoutDragon01AFus");


    MES::Scene::GetSingleton().isOpen = true;
}

void MES::Scene::StopPositioning()
{
    logger::trace("MES::Scene::StopPositioning");

    RE::PlaySound("VOCShoutDragon01AFus");
    MES::Scene::GetSingleton().isOpen = false;
}

void MES::Scene::PlaceObj()
{
    logger::trace("MES::Scene::PlaceObj");
}