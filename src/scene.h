#pragma once
#include "logger.h"
#include <list>
#include <Windows.h>

namespace MES
{
	class Scene
	{
	public:
		Scene() = default;
		~Scene() = default;
		Scene(const Scene&) = delete;
		Scene(Scene&&) = delete;
		Scene& operator=(const Scene&) = delete;
		Scene& operator=(Scene&&) = delete;

	public:
		class Light
		{
		public:

		};

	public:
		void StartPositioning();
		void StopPositioning();
		void PlaceObj();


		static MES::Scene& GetSingleton();

	private:
		static constexpr uint8_t maxObj = 255;
		static constexpr uint8_t maxLights = 5;
		std::list<Light>         obj;

	public:
		// If the user is currently positioning
		bool isOpen = false;
	};
}
