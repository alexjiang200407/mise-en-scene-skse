#include "scene.h"
#include "utility.h"
#include "serialization.h"
#include "UIManager.h"
#include "event.h"

MES::Scene* MES::Scene::GetSingleton()
{
    static MES::Scene scene;
    return &scene;
}

RE::BSEventNotifyControl MES::Scene::ProcessEvent(RE::InputEvent* const* event, RE::BSTEventSource<RE::InputEvent*>*)
{
    if (!event)
        return RE::BSEventNotifyControl::kContinue;



    logger::info("cunt");
    RE::NiPoint3 crosshairPos = RE::CrosshairPickData::GetSingleton()->collisionPoint;
    RE::hkVector4 vec;
    RE::bhkRigidBody* collider = RE::CrosshairPickData::GetSingleton()->targetCollider;
    RE::TESObjectREFR* player = RE::PlayerCharacter::GetSingleton()->AsReference();
    
    if (!player)
        return RE::BSEventNotifyControl::kContinue;

    logger::info("Player position x: {}, y: {}, z: {}", player->GetPositionX(), player->GetPositionY(), player->GetPositionZ());
    logger::info("Crosshair position x: {}, y: {}, z: {}", crosshairPos.x, crosshairPos.y, crosshairPos.z);
    
    if (collider)
    {
        collider->GetPosition(vec);
    }

    if (vec.quad.m128_f32)
        logger::info("Crosshair position 2 x: {}, y: {}, z: {}", vec.quad.m128_f32[0], vec.quad.m128_f32[1], vec.quad.m128_f32[2]);

    if (!positionedObj || !positionedObj->GetRef())
        return RE::BSEventNotifyControl::kContinue;

    // Move to crosshair
    positionedObj->GetRef()->SetPosition(
        crosshairPos.x, crosshairPos.y, crosshairPos.z
    );

    return RE::BSEventNotifyControl::kContinue;
}

void MES::Scene::PrintAllObj()
{
    logger::info("Printing All Objects");

    if (objs.size() == 0)
    {
        logger::info("No Objects!");
        return;
    }

    // Prints relevant debug information
    for (uint8_t i = 0; auto& obj : objs)
    {
        logger::info(
            "Object: {}, Address: {}, ID: {:x}, Ref Count: {}", 
            i, fmt::ptr(obj), obj->GetRef()->GetFormID(),
            obj->GetRef()->QRefCount()
        );

        i++;
    }
}

MES::Scene::Scene()
{
    objs.reserve(static_cast<uint16_t>(maxObj));
}

MES::Scene::~Scene()
{
    logger::debug("Destructing Scene!");
}

void MES::Scene::StartPositioning()
{
    logger::trace("MES::Scene::StartPositioning");

    // Creates object next to player
    // dynamic light flicker 0x18fed
    // torch 0x36343
    // Actor Vex 1cd90

     RE::TESObjectREFR* ref = CreateObj(0x1cd90);

     if (!ref)
         return;

     positionedObj = std::make_unique<SceneObj>(ref);
     newObj = true;
     StartPositioning(positionedObj.get());
}

void MES::Scene::StartPositioning(uint8_t index)
{
    newObj = false;
    StartPositioning(objs[index].get());
}

void MES::Scene::StartPositioning(MES::SceneObj* sceneObj)
{
    sceneObj->GetRef()->MoveTo(RE::PlayerCharacter::GetSingleton()->AsReference());

    RE::BSInputDeviceManager::GetSingleton()->AddEventSink<RE::InputEvent*>(this);
}

void MES::Scene::StopPositioning()
{
    logger::trace("MES::Scene::StopPositioning");
    MES::EventProcessor::GetSingleton()->Register();

    if (positionedObj)
    {
        positionedObj->DeleteRef();
        positionedObj.release();
    }
}

void MES::Scene::PlaceObj()
{
    logger::info("Cunt");
    // If user is adding a new scene object not modifying existing scene object
    if (newObj)
    {
        // 1. Adds new object to scene array
        objs.push_back(std::make_unique<MES::SceneObj>(*(positionedObj.get())));

        // 2. Updates user interface
        {
            MES::UIManager* ui = MES::UIManager::GetSingleton();
            ui->GetMenu()->PushItem(positionedObj->GetRef()->GetName());
        }

        newObj = false;
    }

    RE::BSInputDeviceManager::GetSingleton()->RemoveEventSink(this);
    positionedObj.release();
}

void MES::Scene::SaveSceneData(SKSE::SerializationInterface* intfc) const
{
    logger::trace("MES::Scene::SaveSceneData");
    logger::info("Saving scene data!");

    if (!intfc->OpenRecord(Serialization::kMESID, Serialization::kSerializationVersion))
    {
        logger::error("Failed to open Scene Data record!");
        return;
    }

    uint8_t size = static_cast<uint8_t>(objs.size());

    logger::info("Saving size: {} to file.", size);

    // First byte always holds amount of objs
    intfc->WriteRecordData(size);

    if (size == 0)
    {
        logger::info("No objects to save!");
        return;
    }

    for (auto& obj : objs)
    {
        if (!obj->Serialize(intfc))
        {
            return;
        };
    }
}

void MES::Scene::SerializeScene(SKSE::SerializationInterface* intfc, uint8_t objCount)
{
    logger::info("Getting saved scene data!");
    
    
    for (uint8_t i = 0u; i < objCount; i++)
    {
        MES::SceneObj obj;
        RE::FormID fid;
        
        intfc->ReadRecordData(fid);

        RE::TESForm* tesform = RE::TESForm::LookupByID(fid);

        if (tesform != nullptr)
        {
            logger::info(
                "Unserialising SceneObj {} with formid: {:x}",
                i, fid
            );

            obj.SetRef(tesform->AsReference());
        }
        else
        {
            logger::error("Formid {:x} not found!!!", fid);
            return;
        }

        objs.push_back(std::make_unique<SceneObj>(obj));
    }
}

void MES::Scene::ClearScene()
{
    logger::trace("MES::Scene::ClearScene");
    logger::info("Clearing scene data!!");

    // Remove all in game references
    for (auto& obj : objs)
    {
        // Deletes the reference to the object
        obj->DeleteRef();
    }

    objs.clear();

    if (positionedObj)
    {
        positionedObj->DeleteRef();
        positionedObj.release();
    }
}

RE::TESObjectREFR* MES::Scene::CreateObj(RE::TESBoundObject* baseObj)
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
    if (objs.size() >= maxObj)
    {
        char buffer[256];
        snprintf(
            buffer, 256, "Cannot have more than %d scene objects!", maxObj
        );

        RE::DebugNotification(buffer, "UIMenuOKSD");
        return nullptr;
    }

    // Creates new object reference and places it at player
    RE::TESObjectREFR* newObjRef = playerRef->PlaceObjectAtMe(baseObj, true).get();

    // objs.push_back(std::make_unique<SceneObj>(newObjRef));

    // Handles based on the baseObj form type
    switch (baseObj->GetFormType())
    {
    // If NPC toggles its AI
    case RE::FormType::NPC:
    {
        // newObjRef->As<RE::Actor>()->EnableAI(false);
    }
    break;
    }

    logger::info(
        "Created Object! Type: {}, Base ID: {:x}, Ref ID: {:x},",
        RE::FormTypeToString(baseObj->GetFormType()), baseObj->GetFormID(),
        newObjRef->GetFormID()
    );

    return newObjRef;
}

RE::TESObjectREFR* MES::Scene::CreateObj(RE::FormID baseId)
{
    RE::TESForm* obj = RE::TESForm::LookupByID(baseId);

    if (!obj)
    {
        logger::warn("baseID: {:x} doesn't exist!", baseId);
        return nullptr;
    }

    return CreateObj(obj->As<RE::TESBoundObject>());
}

