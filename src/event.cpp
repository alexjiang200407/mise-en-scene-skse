#include "event.h"

void MES::EventProcessor::Register()
{
	RE::ScriptEventSourceHolder::GetSingleton()->AddEventSink<RE::TESActivateEvent>(this);
	RE::BSInputDeviceManager::GetSingleton()->AddEventSink<RE::InputEvent*>(this);
}

void MES::EventProcessor::Unregister()
{
	RE::ScriptEventSourceHolder::GetSingleton()->RemoveEventSink<RE::TESActivateEvent>(this);
	RE::BSInputDeviceManager::GetSingleton()->BSTEventSource::RemoveEventSink(this);
}

RE::BSEventNotifyControl MES::EventProcessor::ProcessEvent(
	const RE::TESActivateEvent* event, 
	RE::BSTEventSource<RE::TESActivateEvent>*
)
{
	RE::TESObjectREFR* action = event->actionRef.get();
	RE::TESObjectREFR* activated = event->objectActivated.get();

	if (!action || !activated)
		return RE::BSEventNotifyControl::kContinue;

	// If player
	if (action->IsPlayerRef())
	{
		logger::info("{} activated {}", action->GetBaseObject()->GetName(),
			activated->GetBaseObject()->GetName()
		);
	}

	return RE::BSEventNotifyControl::kContinue;
}

RE::BSEventNotifyControl MES::EventProcessor::ProcessEvent(
	RE::InputEvent* const*               pEvent, 
	RE::BSTEventSource<RE::InputEvent*>*
)
{
	if (
		!pEvent || !*pEvent || 
		(*pEvent)->GetEventType() != RE::INPUT_EVENT_TYPE::kButton
	)
		return RE::BSEventNotifyControl::kContinue;



	RE::InputEvent*  event = *pEvent;
	RE::ButtonEvent* buttonEvt = event->AsButtonEvent();
	uint32_t         dxScancode = buttonEvt->GetIDCode();

	// Positioning menu if L (38) is held
	if (
		dxScancode == 38 &&
		buttonEvt->IsUp() &&
		!MES::UIManager::GetSingleton()->isOpen
	)
	{
		if (!UIManager::GetSingleton()->OpenMenu())
			return RE::BSEventNotifyControl::kContinue;
	}
	// If E (18) is pressed then places down the light
	else if (
		MES::UIManager::GetSingleton()->isOpen && dxScancode == 18 &&
		buttonEvt->IsPressed() && !buttonEvt->IsRepeating()
	)
	{
		// If currently positioning scene object place the object down
		if (MES::Scene::GetSingleton()->GetPositioned().get())
		{
			MES::Scene::GetSingleton()->PlaceProp();
		}

		// If not currently positioning then start positioning
		else
			MES::Scene::GetSingleton()->StartPositioning();

	}

	// If delete (211) is pressed, removes every object spawned
	else if (
		MES::UIManager::GetSingleton()->isOpen && dxScancode == 211 &&
		buttonEvt->heldDownSecs >= 0.5f && buttonEvt->IsUp()
	)
	{
		MES::Scene::GetSingleton()->ClearScene();
	}

#ifdef _DEBUG
	else if (
		MES::UIManager::GetSingleton()->isOpen && dxScancode == 199 &&
		buttonEvt->IsUp()
	)
	{
		MES::Scene::GetSingleton()->PrintAllProp();
	}
#endif

	return RE::BSEventNotifyControl::kContinue;
}

MES::EventProcessor* MES::EventProcessor::GetSingleton()
{
	static MES::EventProcessor singleton;

	return &singleton;
}
