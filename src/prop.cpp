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

void MES::Prop::DeleteRef()
{
    using FLAGS = RE::TESObjectREFR::InGameFormFlag;
    // Marks reference for deletion 
    // it is cleared next time a save is loaded
    logger::info("Deleting ref {:x}", pRef->formID);

    pRef->Disable();
    pRef->SetDelete(true);
}

void MES::Prop::MoveToLooking()
{
    RE::PlayerCharacter* player = RE::PlayerCharacter::GetSingleton();
    RE::NiPoint3         collision = RE::CrosshairPickData::GetSingleton()->collisionPoint;
    RE::NiPoint3         origin;
    RE::NiPoint3         directionVec;

    player->GetEyeVector(origin, directionVec, false);

    directionVec *= 50.0f;

    // X metres ahead of player
    RE::NiPoint3 lookingAt = origin + directionVec;

    if (
        pRef->GetParentCell() != player->GetParentCell() ||
        pRef->GetWorldspace() != player->GetWorldspace()
    )
    {
        pRef->MoveTo(player);
    }

    // Move to the collision point if there is something blocking line of sight
    if (collision.GetDistance(origin) < lookingAt.GetDistance(origin))
    {
        pRef->SetPosition(collision);
    }
    // Move to where player is looking at
    else
    {
        pRef->SetPosition(lookingAt);
    }
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