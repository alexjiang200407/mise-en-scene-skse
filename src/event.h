#include "logger.h"
#include "lights.h"

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

		// Returns singleton class
		static EventProcessor& GetSingleton();
	};


	class JournalMenu
		:
		public RE::JournalMenu
	{
		class RemapHandler 
			: 
			public RE::BSTEventSink<RE::InputEvent*>
		{
			// Remaps the open journal
			RE::BSEventNotifyControl ProcessEvent_Hook(
				RE::InputEvent** ppEvent, RE::BSTEventSource<RE::InputEvent**>* pSrc
			);


			using ProcessEvent_t = decltype(&ProcessEvent_Hook);
			static inline REL::Relocation<ProcessEvent_t> _ProcessEvent;
		};
	};
}