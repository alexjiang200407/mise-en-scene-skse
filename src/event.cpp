#include "event.h"

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
	RE::BSTEventSource<RE::InputEvent*>* pSrc
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
		!MES::Lighting::GetSingleton().isLocked
	)
	{
		MES::Lighting::GetSingleton().StartPositioning();
	}

	// Exits out of positioning menu if escape (1) is pressed
	else if (dxScancode == 1 && MES::Lighting::GetSingleton().isLocked)
	{
		// Prevents the journal from opening
		PreventUIMsg(RE::JournalMenu::MENU_NAME, RE::UI_MESSAGE_TYPE::kShow);

		// Stops the positioning
		MES::Lighting::GetSingleton().StopPositioning();
	}

	// If E (18) is pressed then places down the light
	else if (MES::Lighting::GetSingleton().isLocked && dxScancode == 18)
	{
		MES::Lighting::GetSingleton().PlaceObj();
	}

	return RE::BSEventNotifyControl::kContinue;
}

bool MES::InputEventProcessor::PreventUIMsg(std::string_view menu, RE::UI_MESSAGE_TYPE type)
{
	RE::UIMessageQueue* uiMsgQueue = RE::UIMessageQueue::GetSingleton();
	bool                flag = false;

	for (auto& msg : uiMsgQueue->messagePool)
	{
		if (
			msg.menu == menu &&
			msg.type == type
		)
		{
			logger::info("Found {} message!!", menu);

			RE::UIMessage* pMsg = &msg;

			if (uiMsgQueue->messages.Pop(&pMsg))
				logger::info("Successfully removed {} open event!!! YAY", menu);
			else
				logger::error("Epic fail, couldn't remove {} open event.", menu);

			flag = true;
			break;
		}
	}

	if (!flag)
		logger::warn("Did not remove any {} event!!!", menu);


	return flag;
}

MES::InputEventProcessor& MES::InputEventProcessor::GetSingleton()
{
	static MES::InputEventProcessor singleton;

	return singleton;
}
