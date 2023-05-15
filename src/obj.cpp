#include "obj.h"

MES::SceneObj::SceneObj(RE::TESObjectREFR* pRef)
	:
	pRef(pRef)
{}

RE::TESObjectREFR* MES::SceneObj::GetRef() const
{
	return pRef;
}

void MES::SceneObj::SetRef(RE::TESObjectREFR* ref)
{
    pRef = ref;
}

bool MES::SceneObj::Serialize(SKSE::SerializationInterface* intfc) const
{
    logger::info(
        "Saving SceneObj with formId {:x}", 
        this->GetRef()->GetFormID()
    );

    if (
        !intfc->WriteRecordData(this->GetRef()->GetFormID())
    )
    {
        logger::error("Failed to save data.");
        return false;
    };

    return true;
}