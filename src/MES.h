#pragma once
#include "logger.h"

namespace MES 
{
    // Initialises Mise-En-Scene
    void Init();
    void RegisterEventHandler();

    // Process SKSE system messages, save / load game etc.
    void ProcessSysMessages(SKSE::MessagingInterface::Message* msg);
}
