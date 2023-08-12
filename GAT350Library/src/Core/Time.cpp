#include "Core.h"
#include "Time.h"

#include "Renderer/RendererAPI.h"
#include <chrono>

double Engine::Time::m_DeltaTime = 0.0f;
double Engine::Time::m_LastFrameTime = 0.0f;

namespace Engine
{
	using namespace std::chrono;
	double Time::GetTime()
	{
		return duration<double>(system_clock::now().time_since_epoch()).count();
	}

	void Time::UpdateDeltaTime()
	{
		double time = GetTime();
		m_DeltaTime = time - m_LastFrameTime;
		m_LastFrameTime = time;
	}

	void Time::Init()
	{
		m_LastFrameTime = GetTime();
	}
	
}
