#include "logger.h"
#include "scene.h"

namespace MES
{
	class InputEventProcessor
		: 
		public RE::BSTEventSink<RE::TESActivateEvent>,
		public RE::BSTEventSink<RE::InputEvent*>
	{
	public:
		InputEventProcessor() = default;
		~InputEventProcessor() = default;
		InputEventProcessor(const InputEventProcessor&) = delete;
		InputEventProcessor(InputEventProcessor&&) = delete;
		InputEventProcessor& operator=(const InputEventProcessor&) = delete;
		InputEventProcessor& operator=(InputEventProcessor&&) = delete;

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
		static InputEventProcessor& GetSingleton();
	};

}