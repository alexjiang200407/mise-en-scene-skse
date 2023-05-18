#include "UIManager.h"


void MES::UIManager::Register() const
{
	MESUI::Register();
	RE::UI::GetSingleton()->AddEventSink<RE::MenuOpenCloseEvent>(UIManager::GetSingleton());
}

bool MES::UIManager::PreventUIMsg(const std::string_view menu, const RE::UI_MESSAGE_TYPE type)
{
	logger::trace("MES::EventProcessor::PreventUIMsg");

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

RE::GPtr<MES::MESUI> MES::UIManager::GetMESMenu()
{
	auto* ui = RE::UI::GetSingleton();
	
	return ui? ui->GetMenu<MESUI>(MESUI::MENU_NAME) : nullptr;
}

bool MES::UIManager::OpenMenu()
{
	RE::UI* ui = RE::UI::GetSingleton();

	// Checks if another menu is open
	for (auto& menu : ui->menuStack)
	{
		if (
			!menu->AlwaysOpen() ||
			menu->Modal() ||
			menu->FreezeFramePause()
		)
		{
			logger::warn("Another menu is open, cannot open MES menu!!!");
			return false;
		}
	}

	auto* msgQueue = RE::UIMessageQueue::GetSingleton();

	if (!msgQueue)
		return false;

	// Adds show event to message queue
	msgQueue->AddMessage(
		MESUI::MENU_NAME, 
		RE::UI_MESSAGE_TYPE::kShow, nullptr
	);

	isOpen = true;
	return true;
}

bool MES::UIManager::CloseMenu()
{
	RE::PlaySound("VOCShoutDragon01AFus");

	auto msgQueue = RE::UIMessageQueue::GetSingleton();

	// Adds show event to message queue
	msgQueue->AddMessage(
		MESUI::MENU_NAME, RE::UI_MESSAGE_TYPE::kHide, nullptr
	);

	isOpen = false;

	return true;
}
	
RE::BSEventNotifyControl MES::UIManager::ProcessEvent(
	const RE::MenuOpenCloseEvent* event, 
	RE::BSTEventSource<RE::MenuOpenCloseEvent>* src
)
{
	if (!event)
		return RE::BSEventNotifyControl::kContinue;

	if (event->menuName == MESUI::MENU_NAME)
	{
		if (event->opening == isOpen)
		{
			logger::info("Process Event is right!");
		}
		else
		{
			logger::info("Process event is wrong!");
		}
	}
	else
	{
		logger::info("Not my menu!");
	}

	return RE::BSEventNotifyControl::kContinue;
}

MES::UIManager* MES::UIManager::GetSingleton()
{
	static UIManager singleton;
	return &singleton;
}


