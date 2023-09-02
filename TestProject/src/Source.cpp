#include "Core/Core.h"

#include "Platform/Windows/Win.h"
#include "Core/Window.h"
#include "Core/VException.h"
#include "Application.h"
#include "Util/Performance.h"

#include "MainWindow.h"

//int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
int main(int argc, char** argv)
{

	try
	{
		Engine::Application app = Engine::Application();
		Engine::Instrumentor::Get().RecordData(true);
		Engine::Instrumentor::Get().BeginSession("Runtime", "runtime.json");
		int exitCode = app.Run();
		Engine::Instrumentor::Get().EndSession();
		return exitCode;
	}
	catch (const VException& e)
	{
		MessageBoxA(nullptr, e.what(), e.GetType(), MB_OK | MB_ICONEXCLAMATION);
	}
	catch (const std::exception& e)
	{
		MessageBoxA(nullptr, e.what(), "Standard Exception", MB_OK | MB_ICONEXCLAMATION);
	}
	catch (...)
	{
		MessageBoxA(nullptr, "No details available", "Unknown Exception", MB_OK | MB_ICONEXCLAMATION);
	}

	return -1;
}