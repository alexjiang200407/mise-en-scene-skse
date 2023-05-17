#pragma once
#include "scene.h"

namespace Serialization
{
	enum : uint32_t
	{
		kSerializationVersion = 420,
		kMESID = '6969'
	};

	void LoadCallback(SKSE::SerializationInterface* intfc);

	void SaveCallback(SKSE::SerializationInterface* intfc);

	void RevertCallback(SKSE::SerializationInterface*);
}