#pragma once
#include "logger.h"
#include "scene.h"
#include "utility.h"
#include "UIManager.h"

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

		void Register();
		void Unregister();
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
		static EventProcessor* GetSingleton();
	};

}