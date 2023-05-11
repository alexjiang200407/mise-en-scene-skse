#include "lights.h"

MES::Lighting &MES::Lighting::GetSingleton()
{
    static MES::Lighting lighting;
    return lighting;
}

void MES::Lighting::StartPositioning()
{
    logger::trace("MES::Lighting::StartPositioning");

    // If another menu is open then doesn't do anything
    RE::UI* ui = RE::UI::GetSingleton();

    for (auto &menu : ui->menuStack)
    {
        if (
            !menu->AlwaysOpen() || 
            menu->Modal() || 
            menu->FreezeFramePause()
        )
        {
            logger::warn("Another menu is open, cannot start positioning!!!");
            return;
        }
    }

    RE::PlaySound("VOCShoutDragon01AFus");
    
    int formId = 0x75C32;

    // BSLight for ambient??
    // BSShadowLight
    // BGSLightingTemplate
    // BGSVolumetricLighting

    // RE::TESForm* light = RE::TESForm::LookupByID<RE::BGSLightingTemplate>(formId);
    // RE::TESForm::GetAllForms();

    RE::TESForm* lightBase = RE::TESForm::LookupByID(formId);


    if (!lightBase)
    {
        logger::warn("Light doesn't fucking exist");
        return;
    }

    MES::Lighting::GetSingleton().isLocked = true;
}

void MES::Lighting::StopPositioning()
{
    logger::trace("MES::Lighting::StopPositioning");
    MES::Lighting::GetSingleton().isLocked = false;
}

void MES::Lighting::PlaceObj()
{
    logger::trace("MES::Lighting::PlaceObj");
}
