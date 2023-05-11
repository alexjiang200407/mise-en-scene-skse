#pragma once
#include <spdlog/sinks/basic_file_sink.h>


namespace logger = SKSE::log;

namespace MES
{
    // Setup SKSE logging, returns true if successful, false if unsuccessful
    bool SetupLog();
}
