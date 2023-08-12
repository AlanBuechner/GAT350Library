#pragma once
#include "Core/Window.h"

namespace Engine
{
	class Application
	{
	public:
		Application();

		int Run();

	protected:
		void OnCreate();
		void OnUpdate();
		void OnDestroy();
	};
}