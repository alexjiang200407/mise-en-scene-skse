#pragma once
#include "UI.h"
#include "logger.h"
#include "utility.h"

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
		bool PreventUIMsg(
			const std::string_view menu, const RE::UI_MESSAGE_TYPE type
		);

		RE::GPtr<MES::MESUI> GetMESMenu();

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