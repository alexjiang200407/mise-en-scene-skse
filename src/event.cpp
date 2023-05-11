#include "event.h"

RE::BSEventNotifyControl MES::EventProcessor::ProcessEvent(
	const RE::TESActivateEvent* event, 
	RE::BSTEventSource<RE::TESActivateEvent>*
)
{
	auto* action = event->actionRef.get();
	auto* activated = event->objectActivated.get();

	if (!action || !activated)
		return RE::BSEventNotifyControl::kContinue;

	// If player
	if (action->IsPlayerRef())
	{
		logger::info("{} activated {}", action->GetBaseObject()->GetName(),
			activated->GetBaseObject()->GetName());
	}

	return RE::BSEventNotifyControl::kContinue;
}

RE::BSEventNotifyControl MES::EventProcessor::ProcessEvent(
	RE::InputEvent* const* pEvent, 
	RE::BSTEventSource<RE::InputEvent*>* pSrc
)
{
	// Does nothing if the event is nullptr
	if (!pEvent || !*pEvent)
		return RE::BSEventNotifyControl::kContinue;

	RE::InputEvent* event = *pEvent;

	// If button event
	if (event->GetEventType() == RE::INPUT_EVENT_TYPE::kButton)
	{
		RE::ButtonEvent* buttonEvent = event->AsButtonEvent();
		uint32_t dxScancode = buttonEvent->GetIDCode();

		// Positioning light menu if L (38) is held
		if (
			dxScancode == 38 &&
			buttonEvent->heldDownSecs >= 0.5f &&
			!MES::Lighting::GetSingleton().isLocked
		)
		{
			MES::Lighting::GetSingleton().StartPositioning();
		}
		// Exits out of positioning menu if escape (1) is pressed
		else if (dxScancode == 1 && MES::Lighting::GetSingleton().isLocked)
		{
			//pSrc->pendingUnregisters.clear();
			/*pSrc->sinks.clear();*/

			MES::Lighting::GetSingleton().StopPositioning();
		}

	}

	return RE::BSEventNotifyControl::kContinue;
}

MES::EventProcessor& MES::EventProcessor::GetSingleton()
{
	static MES::EventProcessor singleton;

	return singleton;
}

RE::BSEventNotifyControl MES::JournalMenu::RemapHandler::ProcessEvent_Hook(
	RE::InputEvent** ppEvent, RE::BSTEventSource<RE::InputEvent**>* pSrc
)
{
	// If not a button press then does nothing
	if (
		!ppEvent || !*ppEvent || 
		(*ppEvent)->GetEventType() != RE::INPUT_EVENT_TYPE::kButton
	)
	{
		return RE::BSEventNotifyControl::kContinue;
	}

	// If the user is currently positioning then doesn't open journal
	if (MES::Lighting::GetSingleton().isLocked == true)
	{
		
	}

	return RE::BSEventNotifyControl::kContinue;
}
