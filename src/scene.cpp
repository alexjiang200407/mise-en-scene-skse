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
   
    positionedProp->MoveToLooking();
    
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
    logger::trace("MES::Scene::StartPositioning");
    OpenChooseObjMenu();
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
    StopPositioning();
}

void MES::Scene::DeleteProp(uint8_t index)
{
    Prop* toBeDeleted = props[index].get();

    if (positionedProp && positionedProp->GetRef() == toBeDeleted->GetRef())
        StopPositioning();


    props[index]->DeleteRef();
    props.erase(std::next(props.begin(), index));
}

RE::TESObjectREFR* MES::Scene::CreateProp(RE::TESBoundObject* baseObj)
{
    logger::trace("MES::Scene::CreateProp");
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

    // Random Test Cell
    RE::TESForm* cell = RE::TESForm::LookupByID(0x4e227);

    if (!cell)
    {
        logger::error("Cell not found {}", 0x4e227);
        return nullptr;
    }

    // Creates new object reference and places it at a test cell
    auto newPropRef = RE::TESDataHandler::GetSingleton()->CreateReferenceAtLocation(
        baseObj, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f}, cell->As<RE::TESObjectCELL>(), nullptr,
        nullptr, nullptr, {}, true, false
    ).get().get();

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

void MES::Scene::OpenChooseObjMenu()
{
    std::vector<std::string> buttonText;

    // buttonText for the object types
    {
        MES::Scene* scene = MES::Scene::GetSingleton();

        buttonText.reserve(scene->GetBoundObjs().size());

        // Shows all the possible types
        for (auto& type : scene->GetBoundObjs())
        {
            buttonText.push_back(type.typeStr);
        }
    }

    buttonText.push_back("Cancel");
    
    MES::SkyrimMessageBox::Show("Choose Type:", buttonText, [](unsigned int type) {
        MES::Scene* scene = MES::Scene::GetSingleton();

        // Does nothing if selected index is greater than amount of object types
        // This includes selecting back
        if (type >= scene->GetBoundObjs().size())
        {
            MES::SkyrimMessageBox::Hide();
            return;
        }

        
        // Now choose the object
        {

            std::vector<std::string> buttonText;

            buttonText.reserve(scene->GetBoundObjs()[type].objs.size());

            // Message
            for (auto& obj : scene->GetBoundObjs()[type].objs)
                buttonText.push_back(std::string{ obj.buttonTxt });

            buttonText.push_back("Cancel");

            // After player chooses an object, start moving it around
            MES::SkyrimMessageBox::Show(scene->GetBoundObjs()[type].msgTxt, buttonText,
            [type](unsigned int obj) 
            {
                MES::Scene* scene = MES::Scene::GetSingleton();

                // Does nothing if selected index is greater than amount of bound objects
                // This includes selecting back

                if (obj >= scene->GetBoundObjs()[type].objs.size())
                {
                    MES::SkyrimMessageBox::Hide();
                    return;
                }

                RE::TESObjectREFR* ref = scene->CreateProp(scene->GetBoundObjs()[type][obj].baseId);

                if (!ref)
                    return;

                scene->GetPositioned() = std::make_unique<Prop>(ref);
                scene->newProp = true;

                MES::SkyrimMessageBox::Hide();

                // Start moving the prop around
                RE::BSInputDeviceManager::GetSingleton()->AddEventSink<RE::InputEvent*>(scene);
            });
        }

    });

}

