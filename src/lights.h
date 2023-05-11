#include "logger.h"
#include <list>

namespace MES
{
	class Lighting
	{
	public:
		Lighting() = default;
		~Lighting() = default;
		Lighting(const Lighting&) = delete;
		Lighting(Lighting&&) = delete;
		Lighting& operator=(const Lighting&) = delete;
		Lighting& operator=(Lighting&&) = delete;


	public:
		class Light
		{
		public:

		};

	public:
		// Creates a light source
		void StartPositioning();
		void StopPositioning();
		static MES::Lighting &GetSingleton();

	private:
		static constexpr int8_t maxLights = 5;

		// Linked list of all the lights
		std::list<Light> lights;

	public:
		// If the user is currently positioning a light
		bool isLocked = false;
	};
}