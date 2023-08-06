#include "Core.h"
#include "Window.h"
#include "Application.h"
#include "Renderer/RendererCommand.h"

#ifdef PLATFORM_WINDOWS
	#include "Platform/Windows/WindowsWindow.h"
#endif // PLATFORM_WINDOWS


#include <functional>

std::vector<Engine::Ref<Engine::Window>> Engine::Window::s_Windows;
std::vector<Engine::Window*> Engine::Window::s_WindowsToRemove;

namespace Engine
{

	Window::Window(WindowProps props) :
		m_NativeWindow(NativeWindow::CreateNativeWindow(props, this))
	{

	}

	Window::~Window()
	{
		delete &m_NativeWindow;
	}

	void Window::Update()
	{
		m_Input.lastx = m_Input.x; m_Input.lasty = m_Input.y;
		m_NativeWindow.PullEvents();

		m_Input.UpdateKeyStates();

		OnUpdate();
	}

	void Window::Resize(int width, int height)
	{
		m_NativeWindow.Resize(width, height);
	}

	void Window::SwapBuffers()
	{
		m_NativeWindow.SwapBuffers();
	}

	void Window::CloseWindow()
	{
		OnClose();
		s_WindowsToRemove.push_back(this);
	}

	float Window::GetAspect()
	{
		return (float)m_NativeWindow.GetProps().width / (float)m_NativeWindow.GetProps().height;
	}

	void Window::UpdateWindows()
	{
		CREATE_PROFILE_FUNCTIONI();
		for (uint32_t i = 0; i < s_Windows.size(); i++)
		{
			s_Windows[i]->Update();
		}
	}

	void Window::SwapWindowsBuffers()
	{
		CREATE_PROFILE_FUNCTIONI();
		for (uint32_t i = 0; i < s_Windows.size(); i++)
		{
			s_Windows[i]->SwapBuffers();
		}
	}

	void Window::RemoveWindows()
	{
		CREATE_PROFILE_FUNCTIONI();
		for(uint32_t wtr = 0; wtr < s_WindowsToRemove.size(); wtr++)
		{
			for (uint32_t w = 0; w < s_Windows.size(); w++)
			{
				if (s_Windows[w].get() == s_WindowsToRemove[wtr]) {
					s_Windows.erase(s_Windows.begin() + w);
					break;
				}
			}
		}
		s_WindowsToRemove.clear();
	}


	NativeWindow& NativeWindow::CreateNativeWindow(Window::WindowProps props, Window* owningWindow)
	{

#ifdef PLATFORM_WINDOWS
		return *(new WindowsWindow(props, owningWindow));
#endif // PLATFORM_WINDOWS

	}

}