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

	// Clears previous scene data
	scene->GetProps().clear();

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
			uint8_t propCount = 0;

			// Gets amount of objects
			intfc->ReadRecordData(propCount);
			
			logger::info("Amount of serialized props: {}", propCount);


			scene->UnserializeScene(intfc, propCount);
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
	scene->SerializeScene(intfc);
}

void Serialization::RevertCallback(SKSE::SerializationInterface*)
{
	logger::trace("Serialization::RevertCallback");
	// MES::Scene::GetSingleton()->PrintAllProp();
	MES::Scene* scene = MES::Scene::GetSingleton();
	scene->GetProps().clear();

}