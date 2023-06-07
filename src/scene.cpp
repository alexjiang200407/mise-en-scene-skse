#include "scene.h"
#include "utility.h"
#include "serialization.h"
#include "UIManager.h"
#include "event.h"
#include "MessageBox.h"

MES::Scene* MES::Scene::GetSingleton()
{
    static MES::Scene scene;
    return &scene;
}

RE::BSEventNotifyControl MES::Scene::ProcessEvent(RE::InputEvent* const* event, RE::BSTEventSource<RE::InputEvent*>*)
{
    if (!event || !(*event))
        return RE::BSEventNotifyControl::kContinue;

    bool doubleDistance = false;

    switch ((*event)->GetEventType())
    {
    case RE::INPUT_EVENT_TYPE::kMouseMove:
    {

    }
    break;
    default:
        return RE::BSEventNotifyControl::kContinue;
    break;
    }
    
    RE::PlayerCharacter* player = RE::PlayerCharacter::GetSingleton();
    RE::NiPoint3         collision = RE::CrosshairPickData::GetSingleton()->collisionPoint;
    RE::NiPoint3         origin;
    RE::NiPoint3         directionVec;

    player->GetEyeVector(origin, directionVec, true);
    
    directionVec *= 150.0f;

    // X metres ahead of player
    RE::NiPoint3 lookingAt = origin + directionVec;

    // Removes gravity
    // positionedProp->GetRef()->DetachHavok(positionedProp->GetRef()->Get3D());
    // positionedProp->GetRef()->SetCollision(false);

    // Move to the collision point if there is something blocking line of sight
    if (collision.GetDistance(origin) < lookingAt.GetDistance(origin))
    {
        positionedProp->GetRef()->SetPosition(collision);
    }
    // Move to where player is looking at
    else
    {
        positionedProp->GetRef()->SetPosition(lookingAt);
    }
       
    
    return RE::BSEventNotifyControl::kContinue; 
}

void MES::Scene::PrintAllProp()
{
    logger::info("Printing All Objects");

    if (props.size() == 0)
    {
        logger::info("No Objects!");
        return;
    }

    // Prints relevant debug information
    for (uint8_t i = 0; auto& prop : props)
    {
        logger::info(
            "Prop: {}, Address: {}, ID: {:x}, Ref Count: {}", 
            i, fmt::ptr(prop), prop->GetRef()->GetFormID(),
            prop->GetRef()->QRefCount()
        );

        i++;
    }
}

MES::Scene::Scene()
{
    props.reserve(static_cast<uint16_t>(maxProps));
}

MES::Scene::~Scene()
{
    logger::debug("Destructing Scene!");
}

void MES::Scene::StartPositioning()
{
    std::vector<std::string> buttonText;
    buttonText.reserve(baseObjIds.size());

    for (auto& id : baseObjIds)
    {
        RE::TESForm* form = RE::TESForm::LookupByID(id);
        buttonText.push_back(std::string{ form->GetName() });
    }

    MES::SkyrimMessageBox::Show("Place Prop", buttonText, [](unsigned int i) {
        MES::Scene* scene = MES::Scene::GetSingleton();
        RE::TESObjectREFR* ref = scene->CreateProp(scene->GetBoundObjs()[i]);

        if (!ref)
            return;

        scene->GetPositioned() = std::make_unique<Prop>(ref);
        scene->newProp = true;
        RE::BSInputDeviceManager::GetSingleton()->AddEventSink<RE::InputEvent*>(scene);

        if (RE::UI::GetSingleton()->GetMenu(RE::MessageBoxMenu::MENU_NAME))
        {
            RE::UIMessageQueue* msgQueue = RE::UIMessageQueue::GetSingleton();
            msgQueue->AddMessage(RE::MessageBoxMenu::MENU_NAME, RE::UI_MESSAGE_TYPE::kHide, nullptr);
        };

    });

}

void MES::Scene::StartPositioning(uint8_t index)
{
    logger::trace("MES::Scene::StartPositioning");
    newProp = false;
    positionedProp = std::make_unique<Prop>(props[index].get()->GetRef());
    RE::BSInputDeviceManager::GetSingleton()->AddEventSink<RE::InputEvent*>(this);
}

void MES::Scene::StopPositioning()
{
    logger::trace("MES::Scene::StopPositioning");

    // If user is currently positioning an object then
    if (positionedProp)
    {
        // If new object then clears the object
        if (newProp)
        {
            positionedProp->DeleteRef();
            newProp = false;
        }

        // Frees the pointer to the positioned object
        positionedProp.release();
    }

    RE::BSInputDeviceManager::GetSingleton()->RemoveEventSink(this);
}

void MES::Scene::PlaceProp()
{
    logger::trace("MES::Scene::PlaceProp");

    // If user is adding a new scene object not modifying existing scene object
    if (newProp)
    {
        // 1. Adds new object to scene array
        props.push_back(std::make_unique<MES::Prop>(*(positionedProp.get())));

        // 2. Updates user interface
        {
            MES::UIManager* ui = MES::UIManager::GetSingleton();

            ui->GetMenu()->PushItem(positionedProp->GetRef()->GetName());
        }

        newProp = false;
    }
    StopPositioning();
}

void MES::Scene::SerializeScene(SKSE::SerializationInterface* intfc) const
{
    logger::trace("MES::Scene::SerializeScene");

    if (!intfc->OpenRecord(Serialization::kMESID, Serialization::kSerializationVersion))
    {
        logger::error("Failed to open Scene Data record!");
        return;
    }

    uint8_t size = static_cast<uint8_t>(props.size());

    logger::info("Saving size: {} to file.", size);

    // First byte always holds amount of props
    intfc->WriteRecordData(size);

    if (size == 0)
    {
        logger::info("No objects to save!");
        return;
    }

    for (auto& obj : props)
    {
        if (!obj->Serialize(intfc))
        {
            return;
        };
    }
}

void MES::Scene::UnserializeScene(SKSE::SerializationInterface* intfc, uint8_t objCount)
{
    logger::trace("MES::Scene::UnserializeScene");
    
    for (uint8_t i = 0u; i < objCount; i++)
    {
        MES::Prop    obj;
        RE::FormID   fid;
        RE::TESForm* tesform;

        // Get reference ids of saved props
        intfc->ReadRecordData(fid);

        // Checks to see if ref ids exists
        tesform = RE::TESForm::LookupByID(fid);

        if (tesform)
        {
            logger::info(
                "Unserialising Prop {} with formid: {:x}",
                i, fid
            );

            // Adds the stored ref id to the props vector
            obj.SetRef(tesform->AsReference());
            props.push_back(std::make_unique<Prop>(obj));
        }
        else
        {
            logger::error("Formid {:x} not found!!!", fid);
        }

    }
}

void MES::Scene::ClearScene()
{
    logger::trace("MES::Scene::ClearScene");

    // Remove all in game references
    for (auto& prop : props)
        prop->DeleteRef();

    // Clears the container of props
    props.clear();

    // Also clears the currently positioned object
    // TODO test if not StopPositioning works
    StopPositioning();
}

RE::TESObjectREFR* MES::Scene::CreateProp(RE::TESBoundObject* baseObj)
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
    if (props.size() >= maxProps)
    {
        std::ostringstream oss;
        oss << "Cannot have more than " << maxProps 
            << " scene objects!";

        RE::DebugNotification(oss.str().c_str(), "UIMenuOKSD");
        return nullptr;
    }

    // Creates new object reference and places it at player
    RE::TESObjectREFR* newPropRef = playerRef->PlaceObjectAtMe(baseObj, true).get();

    // Does something based on the prop's form type
    switch (baseObj->GetFormType())
    {
    case RE::FormType::Furniture:
    case RE::FormType::AlchemyItem:
    {

    }
    break;
    case RE::FormType::Light:
    {
        newPropRef->SetCollision(false);
    }
    break;
    }

    logger::info(
        "Created Object! Type: {}, Base ID: {:x}, Ref ID: {:x},",
        RE::FormTypeToString(baseObj->GetFormType()), baseObj->GetFormID(),
        newPropRef->GetFormID()
    );

    return newPropRef;
}

RE::TESObjectREFR* MES::Scene::CreateProp(RE::FormID baseId)
{
    RE::TESForm* obj = RE::TESForm::LookupByID(baseId);

    if (!obj)
    {
        logger::warn("baseID: {:x} doesn't exist!", baseId);
        return nullptr;
    }

    return CreateProp(obj->As<RE::TESBoundObject>());
}

