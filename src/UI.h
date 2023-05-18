#pragma once
#include "logger.h"


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
		constexpr static std::string_view GetName()
		{
			return MENU_NAME;
		}

		constexpr static std::int8_t GetPriority();
		static RE::stl::owner<RE::IMenu*> Creator() { return new MESUI(); }

		// RE::GPtr<RE::GFxMovieView> GetView();

		RE::UI_MESSAGE_RESULTS ProcessMessage(RE::UIMessage& msg) override;

		void AdvanceMovie(float a_interval, std::uint32_t a_currentTime) override;


	private:
		class Logger : public RE::GFxLog
		{
		public:
			void LogMessageVarg(LogMessageType, const char* a_fmt, std::va_list a_argList) override
			{
				std::string fmt(a_fmt ? a_fmt : "");
				while (!fmt.empty() && fmt.back() == '\n') {
					fmt.pop_back();
				}

				std::va_list args;
				va_copy(args, a_argList);
				std::vector<char> buf(static_cast<std::size_t>(std::vsnprintf(0, 0, fmt.c_str(), a_argList) + 1));
				std::vsnprintf(buf.data(), buf.size(), fmt.c_str(), args);
				va_end(args);

				logger::info("{}: {}"sv, MESUI::GetName(), buf.data());
			}
		};
	public:
		constexpr static std::string_view FILE_NAME{ "MESUI" };
		constexpr static std::string_view MENU_NAME{ "MESUI" };
		constexpr static std::int8_t SORT_PRIORITY{ 0 };

		//RE::GPtr<RE::GFxMovieView> view;

		//bool isOpen = false;
	};
}
