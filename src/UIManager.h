#pragma once
#include "scene.h"
#include "UI.h"
#include "logger.h"
#include "utility.h"
#include <sstream>

namespace MES
{
	class UIManager
		:
		public RE::BSTEventSink<RE::MenuOpenCloseEvent>
	{
	public:
		UIManager() = default;
		UIManager(const UIManager&) = delete;
		UIManager(UIManager&&) = delete;
		UIManager& operator=(const UIManager&) = delete;
		UIManager& operator=(UIManager&&) = delete;

	public:
		void Register() const;
		RE::GPtr<MES::MESUI> GetMenu();

		bool OpenMenu();
		bool CloseMenu();

		RE::BSEventNotifyControl ProcessEvent(
			const RE::MenuOpenCloseEvent* event, 
			RE::BSTEventSource<RE::MenuOpenCloseEvent>*
		) override;

		static UIManager* GetSingleton();

	public:
		bool isOpen = false;

	};

}