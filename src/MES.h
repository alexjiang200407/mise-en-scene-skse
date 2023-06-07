#pragma once
#include "logger.h"
#include "event.h"
#include "serialization.h"
#include "UIManager.h"

namespace MES 
{
    // Initialises Mise-En-Scene
    void Init();

    // Registers Event Handlers
    void RegisterEventHandler();

    // Unregisters Event Handlers
    void UnregisterEventHandler();

    // Process SKSE system messages, save / load game etc.
    void ProcessSysMessages(SKSE::MessagingInterface::Message* msg);

    // Get the BoundObjects / base object IDs saved in json file
    void GetSavedBaseObjIds();


    // Gets the plugin compile index.
    // Returns 0 if not found
    uint8_t GetPluginCompileIndex();
}
