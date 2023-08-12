#include "Core.h"
#include "Window.h"
#include "Renderer/RendererCommand.h"

#ifdef PLATFORM_WINDOWS
	#include "Platform/Windows/WindowsWindow.h"
#endif // PLATFORM_WINDOWS


#include <functional>

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
	}

	float Window::GetAspect()
	{
		return (float)m_NativeWindow.GetProps().width / (float)m_NativeWindow.GetProps().height;
	}

	bool Window::CloseRequested()
	{
		return m_NativeWindow.CloseRequested();
	}

	NativeWindow& NativeWindow::CreateNativeWindow(Window::WindowProps props, Window* owningWindow)
	{

#ifdef PLATFORM_WINDOWS
		return *(new WindowsWindow(props, owningWindow));
#endif // PLATFORM_WINDOWS

	}

}