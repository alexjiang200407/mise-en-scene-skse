#include "serialization.h"
#include "logger.h"

void Serialization::LoadCallback(SKSE::SerializationInterface* intfc)
{
	logger::trace("Serialization::LoadCallback");
	uint32_t type;
	uint32_t version;

	// Length in bits of serialized data
	uint32_t length;
	MES::Scene* scene = MES::Scene::GetSingleton();
	// bool loaded = false;

	while (intfc->GetNextRecordInfo(type, version, length))
	{
		if (version != Serialization::kSerializationVersion)
		{
			logger::error("Loaded data is out of data!");
			continue;
		}

		if (type == Serialization::kMESID)
		{
			// Deserialize Load
			logger::info("type: {}, version: {}, length: {}", type, version, length);
			uint8_t objCount = 0;

			// Gets amount of objects
			intfc->ReadRecordData(objCount);
			logger::info("Amount of serialized objects: {}", objCount);

			scene->GetSceneData(intfc, objCount);
			break;
		}
		else
		{
			logger::error("Unrecognized signature type {}!", type);
			break;
		}
	}
}

void Serialization::SaveCallback(SKSE::SerializationInterface* intfc)
{
	logger::trace("Serialization::SaveCallback");
	MES::Scene* scene = MES::Scene::GetSingleton();
	scene->SaveSceneData(intfc);
}
