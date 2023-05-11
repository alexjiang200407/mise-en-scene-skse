#pragma once
#include "logger.h"

namespace MES 
{
    // Initialises Skyrim Lighting Manager
    void Init();
    void RegisterEventHandler();

    // Process SKSE system messages, save / load game etc.
    void ProcessSysMessages(SKSE::MessagingInterface::Message* msg);
}
