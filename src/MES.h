#pragma once
#include "logger.h"
#include "event.h"
#include "serialization.h"

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

}
