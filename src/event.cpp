#pragma once
#include "event.h"
#include "utility.h"


RE::BSEventNotifyControl MES::InputEventProcessor::ProcessEvent(
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
			activated->GetBaseObject()->GetName()
		);
	}

	return RE::BSEventNotifyControl::kContinue;
}

RE::BSEventNotifyControl MES::InputEventProcessor::ProcessEvent(
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

	// Positioning light menu if L (38) is held
	if (
		dxScancode == 38 &&
		buttonEvt->heldDownSecs >= 0.5f &&
		!MES::Scene::GetSingleton().isOpen
	)
	{
		MES::Scene::GetSingleton().StartPositioning();
	}

	// Exits out of positioning menu if escape (1) is pressed
	else if (dxScancode == 1 && MES::Scene::GetSingleton().isOpen)
	{
		// Prevents the journal from opening
		PreventUIMsg(RE::JournalMenu::MENU_NAME, RE::UI_MESSAGE_TYPE::kShow);

		// Stops the positioning
		MES::Scene::GetSingleton().StopPositioning();
	}

	// If E (18) is pressed then places down the light
	else if (MES::Scene::GetSingleton().isOpen && dxScancode == 18)
	{
		MES::Scene::GetSingleton().PlaceObj();
	}

	return RE::BSEventNotifyControl::kContinue;
}

// TODO put this in the UI class
bool MES::InputEventProcessor::PreventUIMsg(const std::string_view menu, const RE::UI_MESSAGE_TYPE type)
{
	logger::trace("MES::InputEventProcessor::PreventUIMsg");

	RE::UIMessageQueue* uiMsgQueue = RE::UIMessageQueue::GetSingleton();
	int32_t				typeInt = MES::EnumToInt32(type);

	for (auto& msg : uiMsgQueue->messagePool)
	{
		if (
			msg.menu == menu &&
			msg.type == type
		)
		{
			logger::info("Found {} message with type: {}!!", menu, typeInt);

			RE::UIMessage* pMsg = &msg;

			if (uiMsgQueue->messages.Pop(&pMsg))
			{
				logger::info("Successfully removed {} with event type: {}!!! YAY", menu, typeInt);
				return true;
			}
			else
			{
				logger::error("Epic fail, couldn't remove {} with event type: {}.", menu, typeInt);
				return false;
			}
		}
	}


	logger::warn("Did not remove any {} event!!!", menu);
	return false;
}

MES::InputEventProcessor& MES::InputEventProcessor::GetSingleton()
{
	static MES::InputEventProcessor singleton;

	return singleton;
}
