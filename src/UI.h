#pragma once
#include "logger.h"

// Credit to TrueHud and Skyrim Character Sheet source code
namespace MES
{
	class MESUI
		:
		public RE::IMenu
	{
	public:
		MESUI();
		MESUI(const MESUI&) = default;
		MESUI(MESUI&&) = default;
		MESUI& operator=(const MESUI&) = default;
		MESUI& operator=(MESUI&&) = default;
		~MESUI() = default;

	public:
		static void Register();
		RE::UI_MESSAGE_RESULTS ProcessMessage(RE::UIMessage& msg) override;
		void AdvanceMovie(float a_interval, std::uint32_t a_currentTime) override;
		void Accept(CallbackProcessor* processor) override;

		static void OpenMenu();
		static void CloseMenu();


	// Getters and Setters
	public:
		constexpr static std::string_view GetName() { return MENU_NAME; }
		constexpr static std::int8_t GetPriority() { return SORT_PRIORITY; }
		RE::GPtr<RE::GFxMovieView> GetView() { return view; };
		static RE::stl::owner<RE::IMenu*> Creator() { return new MESUI(); }

	private:
		class Logger : public RE::GFxLog
		{
		public:
			// Logs UI information
			void LogMessageVarg(LogMessageType, const char* a_fmt, std::va_list a_argList) override;
		};

	public:
		constexpr static std::string_view FILE_NAME{ "MESUI" };
		constexpr static std::string_view MENU_NAME{ "MESUI" };
		constexpr static std::int8_t SORT_PRIORITY{ 0 };

		RE::GPtr<RE::GFxMovieView> view;

		bool isOpen = false;
	};
}
