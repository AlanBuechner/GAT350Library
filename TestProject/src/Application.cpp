#include "Application.h"
#include "Core/Time.h"
#include "Renderer/RendererCommand.h"
#include "Util/Performance.h"
#include "MainWindow.h"

namespace Engine
{

	Application::Application()
	{}

	int Application::Run()
	{

		RendererCommand::Init();

		Ref<Window> window = Window::MakeWindow<MainWindow>(1080, 720, L"GAT350");

		OnCreate();

		while (!window->CloseRequested())
		{
			Time::UpdateDeltaTime();
			window->Update();

			OnUpdate();

			window->SwapBuffers();

			DBOUT("FPS: " << Time::GetFPS() << std::endl);
		}

		OnDestroy();

		return 1;
	}

	void Application::OnCreate()
	{

	}

	void Application::OnUpdate()
	{

	}

	void Application::OnDestroy()
	{

	}
}