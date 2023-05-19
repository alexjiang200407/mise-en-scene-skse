#include "UI.h"
#include "utility.h"

MES::MESUI::MESUI()
{
	using FLAGS = RE::UI_MENU_FLAGS;
	auto menu = static_cast<RE::IMenu*>(this);
	menu->depthPriority = GetPriority();
	auto scaleformManager = RE::BSScaleformManager::GetSingleton();

	// Registers user interface
	const bool success =
		scaleformManager->LoadMovieEx(
			menu, FILE_NAME, [&](RE::GFxMovieDef* a_def) -> void {			
				// From Skyrim Character Sheet
				fxDelegate.reset(new RE::FxDelegate);
				fxDelegate->RegisterHandler(this);
				a_def->SetState(
					RE::GFxState::StateType::kExternalInterface, 
					fxDelegate.get()
				);
				fxDelegate->Release();


				a_def->SetState(
					RE::GFxState::StateType::kLog,
					RE::make_gptr<Logger>().get()
				);
			});
	if (!success)
	{
		logger::error("Unable to load MES menu!");
		return;
	}
	else
		logger::error("Loaded MES menu!");

	assert(success);
	
	menuFlags.set(
		FLAGS::kAllowSaving,
		FLAGS::kRequiresUpdate,
		FLAGS::kTopmostRenderedMenu,
		FLAGS::kUsesCursor,
		FLAGS::kPausesGame
	);

	menu->inputContext = Context::kNone;

	view = menu->uiMovie;

	//view->SetVariable("FavoritesMenu.iListItemsShown", 3);
	//view->SetVariable("FavoritesMenu.iMaxItemsShown.", 3);

	RE::FxResponseArgsEx<2> args;
	RE::FxResponseArgsEx<1> args2;

	// Creates array of current objects
	view->CreateArray(&args[0]);

	if (
		!args[0].PushBack("Lighting") ||
		!args[0].PushBack("Props") ||
		!args[0].PushBack("Furniture") ||
		!args[0].PushBack("Environment")
	)
	{
		logger::error("Unable to intialise arguments!");
		return;
	}

	args[1].SetNumber(4.0f);

	args2[0].SetNumber(1);

	fxDelegate->Invoke(view.get(), "PopulateItems", args);
	fxDelegate->Invoke(view.get(), "SetSelectedItem", args2);
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


RE::UI_MESSAGE_RESULTS MES::MESUI::ProcessMessage(RE::UIMessage& msg)
{
	using Type = RE::UI_MESSAGE_TYPE;

	switch (msg.type.get())
	{
	case Type::kShow:
	{
	}
	break;
	case Type::kHide:
	case Type::kForceHide:
	{
	}
	break;
	}

	return RE::IMenu::ProcessMessage(msg);
}

void MES::MESUI::AdvanceMovie(float interval, std::uint32_t currentTime)
{
	if (uiMovie)
	{
		uiMovie->Advance(interval);
	}
}

void MES::MESUI::Accept(CallbackProcessor* processor)
{
	//processor->Process("SetButtonMapping", SetButtonMapping);
	//processor->Process("Cancel", SetButtonMapping);

	processor->Process("PlaySound", [](const RE::FxDelegateArgs& args)
	{
		assert(args[0].IsString());
		assert(args.GetArgCount() == 1);
		RE::PlaySound(static_cast<const char*>(args[0].GetString()));
	});

	processor->Process("CloseMenu", [](const RE::FxDelegateArgs& args)
	{
		// assert(args.GetArgCount() == 1);
		CloseMenu();
	});
}

void MES::MESUI::OpenMenu()
{
	auto* msgQueue = RE::UIMessageQueue::GetSingleton();

	if (!msgQueue)
	{
		logger::error("Message queue not found!");
		return;
	}

	// Adds show event to message queue
	msgQueue->AddMessage(
		MESUI::MENU_NAME, RE::UI_MESSAGE_TYPE::kShow, nullptr
	);
}

void MES::MESUI::CloseMenu()
{
	auto* msgQueue = RE::UIMessageQueue::GetSingleton();

	if (!msgQueue)
	{
		logger::error("Message queue not found!");
		return;
	}

	// Adds show event to message queue
	msgQueue->AddMessage(
		MESUI::MENU_NAME, RE::UI_MESSAGE_TYPE::kHide, nullptr
	);
}

void MES::MESUI::Logger::LogMessageVarg(
	LogMessageType, const char* a_fmt, std::va_list a_argList
)
{
	std::string fmt(a_fmt ? a_fmt : "");
	while (!fmt.empty() && fmt.back() == '\n') 
	{
		fmt.pop_back();
	}

	std::va_list args;
	va_copy(args, a_argList);
	std::vector<char> buf(static_cast<std::size_t>(std::vsnprintf(0, 0, fmt.c_str(), a_argList) + 1));
	std::vsnprintf(buf.data(), buf.size(), fmt.c_str(), args);
	va_end(args);

	logger::info("{}: {}", MESUI::GetName(), buf.data());
}
