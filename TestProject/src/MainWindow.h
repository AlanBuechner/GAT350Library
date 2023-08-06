#pragma once
#include "Core/Window.h"

#include "Renderer/Buffer.h"
#include "Renderer/Mesh.h"
#include "Renderer/Model.h"
#include "Renderer/RendererCommand.h"
#include "Renderer/Shader.h"
#include "Renderer/Camera.h"
#include "Renderer/Texture.h"
#include "Renderer/RenderTarget.h"
#include "Renderer/FrameBuffer.h"


class MainWindow : public Engine::Window
{
public:
	MainWindow(WindowProps props) :
		Window::Window(props)
	{

	}

	~MainWindow()
	{

	}

protected:
	virtual void OnCreate() override;
	virtual void OnUpdate() override;
	virtual void OnClose() override;

private:
	Engine::Ref<Engine::Model> m_Model;
	Engine::Ref<Engine::Shader> m_Shader;

	Engine::Ref<Engine::Camera> m_Camera;
	glm::vec3 m_CameraPosition = { 0.0f, 0.0f, 4.0f };
	Engine::Ref<Engine::ConstantBuffer> m_CameraBuffer;
	Engine::Ref<Engine::ConstantBuffer> m_ModelBuffer;
	Engine::Ref<Engine::ConstantBuffer> m_ModelBuffer2;
	Engine::Ref<Engine::FrameBuffer> m_FrameBuffer;
};