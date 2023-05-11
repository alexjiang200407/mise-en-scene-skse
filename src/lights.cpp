#include "lights.h"

MES::Lighting &MES::Lighting::GetSingleton()
{
    static MES::Lighting lighting;
    return lighting;
}

void MES::Lighting::StartPositioning()
{
    logger::trace("MES::Lighting::StartPositioning");

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
