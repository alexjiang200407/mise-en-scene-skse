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
		logger::info("Loaded MES menu!");

	assert(success);
	
	menuFlags.set(
		FLAGS::kDisablePauseMenu,
		FLAGS::kModal
	);
	
	// Makes it so tabbing will close the menu after
	// messagebox is shown
	menu->inputContext = Context::kFavor;

	view = menu->uiMovie;

	SetSelected(0);

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
		RE::ControlMap::GetSingleton()->ignoreActivateDisabledEvents = true;
	}
	break;
	case Type::kHide:
	case Type::kForceHide:
	{
		RE::ControlMap::GetSingleton()->ignoreActivateDisabledEvents = false;
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
	processor->Process("PlaySound", [](const RE::FxDelegateArgs& args)
	{
		assert(args[0].IsString());
		assert(args.GetArgCount() == 1);
		RE::PlaySound(static_cast<const char*>(args[0].GetString()));
	});

	processor->Process("CloseMenu", [](const RE::FxDelegateArgs& args)
	{
		assert(args.GetArgCount() == 1);
		CloseMenu();
	});

	processor->Process("ItemSelect", [](const RE::FxDelegateArgs& args)
	{
		assert(args.GetArgCount() == 1);
		assert(args[0].IsNumber());

		logger::info("Index {} selected!", static_cast<int8_t>(args[0].GetNumber()));
		// RE::TESObjectREFR* ref = Scene::GetSingleton()->GetProps()[static_cast<uint64_t>(args[0].GetNumber())]->GetRef();
		MES::Scene::GetSingleton()->StartPositioning(static_cast<int8_t>(args[0].GetNumber()));
	});

	processor->Process("DeleteCurrent", [](const RE::FxDelegateArgs& args)
	{
		assert(args.GetArgCount() == 1);
		assert(args[0].IsNumber());

		// No item selected
		if (args[0].GetNumber() < 0)
			return;

		logger::info("Index {} to be deleted!", static_cast<int8_t>(args[0].GetNumber()));

		int index = static_cast<int>(args[0].GetNumber());

		MES::Scene::GetSingleton()->DeleteProp(index);
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


	// Adds show event to Message queue
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

	// Stops positioning
	auto* positioned = MES::Scene::GetSingleton()->GetPositioned().get();

	// Deletes reference if it is new and not placed down yet
	if (positioned && MES::Scene::GetSingleton()->newProp)
	{
		positioned->DeleteRef();
	}

	MES::Scene::GetSingleton()->StopPositioning();
	

	// Adds show event to Message queue
	msgQueue->AddMessage(
		MESUI::MENU_NAME, RE::UI_MESSAGE_TYPE::kHide, nullptr
	);
}

void MES::MESUI::SetSelected(uint8_t index) const
{
	RE::FxResponseArgsEx<1> args;

	args[0].SetNumber(index);
	fxDelegate->Invoke(view.get(), "SetSelected", args);
}

void MES::MESUI::PopulateList(std::vector<const char*>& list) const
{
	std::vector<const char*> strings;

	RE::FxResponseArgsEx<2> args;
	view->CreateArray(&args[0]);

	for (auto& str : list)
	{
		args[0].PushBack(str);
	}

	args[1].SetNumber(static_cast<double>(list.size()));

	fxDelegate->Invoke(view.get(), "PopulateItems", args);
}

void MES::MESUI::PushItem(const char* item) const
{
	RE::FxResponseArgsEx<1> args;

	args[0].SetString(item);

	fxDelegate->Invoke(view.get(), "PushToList", args);
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
