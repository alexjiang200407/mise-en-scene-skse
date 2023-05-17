#include "logger.h"
#include "scene.h"

namespace MES
{
	class EventProcessor
		: 
		public RE::BSTEventSink<RE::TESActivateEvent>,
		public RE::BSTEventSink<RE::InputEvent*>
	{
	public:
		EventProcessor() = default;
		~EventProcessor() = default;
		EventProcessor(const EventProcessor&) = delete;
		EventProcessor(EventProcessor&&) = delete;
		EventProcessor& operator=(const EventProcessor&) = delete;
		EventProcessor& operator=(EventProcessor&&) = delete;

	public:
		// Processes the activate event
		RE::BSEventNotifyControl ProcessEvent(
			const RE::TESActivateEvent* event,
			RE::BSTEventSource<RE::TESActivateEvent>*
		) override;

		// Processes keyboard events
		RE::BSEventNotifyControl ProcessEvent(
			RE::InputEvent* const* pEvent,
			RE::BSTEventSource<RE::InputEvent*>*
		) override;


		// Prevents a UI message from firing
		bool PreventUIMsg(const std::string_view menu, const RE::UI_MESSAGE_TYPE type);

		// Returns singleton class
		static EventProcessor& GetSingleton();
	};

}