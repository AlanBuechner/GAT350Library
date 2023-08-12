#pragma once

namespace Engine
{
	class Application;
}

namespace Engine
{
	class Time
	{
	public:
		Time() {}

		static inline float GetDeltaTime() { return (float)m_DeltaTime; }
		static inline float GetDeltaSeconds() { return (float)m_DeltaTime; }
		static inline float GetDeltaMilliseconds() { return (float)m_DeltaTime * 1000.0f; }

		static inline float GetFPS() { return (float)(1.0 / m_DeltaTime); }

		static double GetTime();

		static void UpdateDeltaTime();

		static void Init();

	protected:
		static double m_DeltaTime;
		static double m_LastFrameTime;
	};
}

typedef Engine::Time Time;