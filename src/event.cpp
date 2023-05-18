#pragma once
#include "event.h"
#include "utility.h"
#include "UIManager.h"


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
		MES::Scene::GetSingleton()->StartPositioning();
	}

	// Exits out of positioning menu if escape (1) is pressed
	else if (dxScancode == 1 && MES::UIManager::GetSingleton()->isOpen)
	{
		// Prevents the journal from opening
		UIManager::GetSingleton()->PreventUIMsg(RE::JournalMenu::MENU_NAME, RE::UI_MESSAGE_TYPE::kShow);

		// Stops the positioning
		MES::Scene::GetSingleton()->StopPositioning();
	}

	// If E (18) is pressed then places down the light
	else if (MES::UIManager::GetSingleton()->isOpen && dxScancode == 18)
	{
		MES::Scene::GetSingleton()->PlaceObj();
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
		MES::Scene::GetSingleton()->PrintAllObj();
	}
#endif

	return RE::BSEventNotifyControl::kContinue;
}

MES::EventProcessor& MES::EventProcessor::GetSingleton()
{
	static MES::EventProcessor singleton;

	return singleton;
}
