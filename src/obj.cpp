#include "obj.h"

MES::SceneObj::SceneObj(RE::TESObjectREFR* ref)
    :
    pRef(ref)
{
}

RE::TESObjectREFR* MES::SceneObj::GetRef() const
{
	return pRef;
}

void MES::SceneObj::SetRef(RE::TESObjectREFR* rhs)
{
    pRef = rhs;
}

bool MES::SceneObj::DeleteRef()
{
    logger::info("Deleting reference");

    // Marks reference for deletion so it is cleared next time a save is loaded
    pRef->Disable();
    pRef->SetDelete(true);

    return true;
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