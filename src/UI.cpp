#include "UI.h"

MES::MESUI::MESUI()
{
	auto menu = static_cast<RE::IMenu*>(this);
	menu->depthPriority = GetPriority();
	auto scaleformManager = RE::BSScaleformManager::GetSingleton();

	if (!scaleformManager)
	{
		logger::error("Unable to open scaleform manager!");
		return;
	}

	const bool success =
		scaleformManager->LoadMovieEx(
			menu, FILE_NAME, [](RE::GFxMovieDef* a_def) -> void {
				a_def->SetState(
					RE::GFxState::StateType::kLog,
					RE::make_gptr<Logger>().get());
			});

	if (!success)
	{
		logger::error("Unable to load MES menu!");
		return;
	}
	else
	{
		logger::error("Loaded MES menu!");
	}
	
	assert(success);

	menuFlags.set(RE::UI_MENU_FLAGS::kAllowSaving);
	menu->inputContext = Context::kNone;
}

void MES::MESUI::Register()
{
	RE::UI* ui = RE::UI::GetSingleton();
	if (ui)
	{
		ui->Register(MENU_NAME, Creator);
		logger::info("Registered menu: {}", MENU_NAME);
	}
}

constexpr std::int8_t MES::MESUI::GetPriority()
{
	return SORT_PRIORITY;
}

RE::UI_MESSAGE_RESULTS MES::MESUI::ProcessMessage(RE::UIMessage& msg)
{
	using Type = RE::UI_MESSAGE_TYPE;

	logger::info("MESUI Processing message!");

	return RE::IMenu::ProcessMessage(msg);
}

void MES::MESUI::AdvanceMovie(float interval, std::uint32_t currentTime)
{
	// ProcessDelegate(interval);
	// Update(interval);

	if (uiMovie)
	{
		uiMovie->Advance(interval);
	}
}
