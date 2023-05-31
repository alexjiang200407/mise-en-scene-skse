#include "UIManager.h"


void MES::UIManager::Register() const
{
	MESUI::Register();
	RE::UI::GetSingleton()->AddEventSink<RE::MenuOpenCloseEvent>(UIManager::GetSingleton());
}

RE::GPtr<MES::MESUI> MES::UIManager::GetMenu()
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

	MES::MESUI::OpenMenu();

	isOpen = true;
	return true;
}

bool MES::UIManager::CloseMenu()
{
	MES::MESUI::CloseMenu();
	
	isOpen = false;

	return true;
}
	
RE::BSEventNotifyControl MES::UIManager::ProcessEvent(
	const RE::MenuOpenCloseEvent* event, 
	RE::BSTEventSource<RE::MenuOpenCloseEvent>*
)
{
	if (!event || event->menuName != MESUI::MENU_NAME)
		return RE::BSEventNotifyControl::kContinue;

	// If the menu is going to be closed
	if (!event->opening && isOpen)
	{
		isOpen = false;
		logger::info("Closing MES menu!");
	}

	// If the menu is going to be opened
	else if (event->opening)
	{
		isOpen = true;
		logger::info("Opening MES menu!");
		
		MES::Scene* scene = MES::Scene::GetSingleton();
		
		std::vector<const char*> list;
		list.reserve(scene->GetObjs().size());

		// TODO? Name as member in the SceneObj class
		for (auto& obj : scene->GetObjs())
		{
			list.push_back(obj->GetRef()->GetName());
		}

		GetMenu()->PopulateList(list);
	}
	

	return RE::BSEventNotifyControl::kContinue;
}

MES::UIManager* MES::UIManager::GetSingleton()
{
	static UIManager singleton;
	return &singleton;
}


