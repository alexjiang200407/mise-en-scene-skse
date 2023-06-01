#include "prop.h"

MES::Prop::Prop(RE::TESObjectREFR* ref)
    :
    pRef(ref)
{
}

RE::TESObjectREFR* MES::Prop::GetRef() const
{
	return pRef;
}

void MES::Prop::SetRef(RE::TESObjectREFR* rhs)
{
    pRef = rhs;
}

bool MES::Prop::DeleteRef()
{
    // Marks reference for deletion 
    // it is cleared next time a save is loaded
    pRef->Disable();
    pRef->SetDelete(true);

    return true;
}

bool MES::Prop::Serialize(SKSE::SerializationInterface* intfc) const
{
    logger::info(
        "Saving Prop with formId {:x}", 
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