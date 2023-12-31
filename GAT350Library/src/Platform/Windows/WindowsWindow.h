#pragma once
#include "Platform/Windows/Win.h"
#include "Core/Window.h"
#include "Renderer/SwapChain.h"

namespace Engine
{

	class WindowsWindow : public NativeWindow
	{
		class WindowClass
		{
		public:
			static const LPCWSTR GetName() noexcept { return wndClassName; }
			static HINSTANCE GetInstance() noexcept { return wndClass.hInst; }

		private:
			WindowClass();
			~WindowClass();
			WindowClass(const WindowClass&) = delete;
			static constexpr const LPCWSTR wndClassName = TEXT("GAT350");
			static WindowClass wndClass;
			HINSTANCE hInst;
		};

	public:
		WindowsWindow(Window::WindowProps props, Window* owningWindow);
		virtual ~WindowsWindow() override;

		virtual Window::WindowProps GetProps() override { return m_Props; }

		virtual void PullEvents() override;

		virtual void Resize(int width, int height) override;
		virtual void CloseWindow() override;

		virtual void SwapBuffers() override;

		virtual bool CloseRequested() override { return m_CloseRequested; }

		virtual SwapChain& GetSwapChain() { return m_Swap; }

	private:
		static LRESULT WINAPI HandleEventSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
		static LRESULT WINAPI HandleEventThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
		LRESULT HandleEvent(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	private:
		bool m_CloseRequested = false;
		Window* m_OwningWindow;
		// window props
		Window::WindowProps m_Props;

		HWND hWnd; // handle to the window
		SwapChain& m_Swap;
	};
}