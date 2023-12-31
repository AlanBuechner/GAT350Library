#include "Core/Core.h"
#include "Core/Time.h"
#include "MainWindow.h"
#include "Application.h"

#include "Renderer/RendererCommand.h"

struct CameraData
{
	glm::mat4 view;
	glm::mat4 viewPorjection;
	glm::vec3 Position;
	float padding;
};

void MainWindow::OnCreate()
{
	m_NativeWindow.GetSwapChain().SetVSync(false);

	m_FrameBuffer = Engine::FrameBuffer::Create({
		Engine::RenderTarget::Create(m_NativeWindow.GetProps().width, m_NativeWindow.GetProps().height, Engine::Texture::Format::RGBA16_FLOAT),
		Engine::RenderTarget::Create(m_NativeWindow.GetProps().width, m_NativeWindow.GetProps().height, Engine::Texture::Format::D24_UNORM_S8_UINT),
	});

	m_Shader = Engine::Shader::Create("Assets/Shaders/TestShader.hlsl");

	m_Camera = Engine::Camera::Create(Engine::Camera::ProjectionType::Perspective, glm::radians(45.0f), 0.01f, 100.0f, GetAspect());
	m_CameraBuffer = Engine::ConstantBuffer::Create(sizeof(CameraData));

	m_Model = Engine::Model::Create("Assets/Models/Sponza/Sponza.gltf");
	//m_Model = Engine::Model::Create("Assets/Models/Suzanne/Suzanne.gltf");
	m_ModelBuffer = Engine::ConstantBuffer::Create(sizeof(glm::mat4));
	m_ModelBuffer2 = Engine::ConstantBuffer::Create(sizeof(glm::mat4));
}

void MainWindow::OnUpdate()
{
	// handle input
	float rotSpeed = 2.0f;
	static glm::vec2 rot = { 0.0f, 0.0f };
	const float deltaTime = Time::GetDeltaTime();

	if (m_Input.GetButtonDown(Engine::Input::RMouse))
	{
		rot.x += m_Input.GetMouseDeltaX() * rotSpeed * deltaTime;
		rot.y += m_Input.GetMouseDeltaY() * rotSpeed * deltaTime;
	}

	glm::mat4 camRot =	glm::rotate(glm::mat4(1.0f), rot.x, { 0.0f, 1.0f, 0.0f }) * 
						glm::rotate(glm::mat4(1.0f), rot.y, { 1.0f, 0.0f, 0.0f });

	if (m_Input.GetKeyDown('A'))
		m_CameraPosition -= (glm::mat3)camRot * (glm::vec3{ 1, 0, 0 } * deltaTime);
	if (m_Input.GetKeyDown('D'))
		m_CameraPosition += (glm::mat3)camRot * (glm::vec3{ 1, 0, 0 } * deltaTime);
	if (m_Input.GetKeyDown('W'))
		m_CameraPosition -= (glm::mat3)camRot * (glm::vec3{ 0, 0, 1 } * deltaTime);
	if (m_Input.GetKeyDown('S'))
		m_CameraPosition += (glm::mat3)camRot * (glm::vec3{ 0, 0, 1 } * deltaTime);
	if (m_Input.GetKeyDown(VK_SPACE))
		m_CameraPosition += (glm::mat3)camRot * (glm::vec3{ 0, 1, 0 } * deltaTime);
	if (m_Input.GetKeyDown(VK_CONTROL))
		m_CameraPosition -= (glm::mat3)camRot * (glm::vec3{ 0, 1, 0 } * deltaTime);

	// update camera
	m_Camera->SetAspect(GetAspect());

	glm::mat4 viewMatrix = glm::inverse(glm::translate(glm::mat4(1.0f), m_CameraPosition) * camRot);
	glm::mat4 projectionMatrix = m_Camera->GetProjectionMatrix();
	glm::mat4 viewPorjectionMatrix = projectionMatrix * viewMatrix;

	CameraData camData = {};
	camData.view = viewMatrix;
	camData.viewPorjection = viewPorjectionMatrix;
	camData.Position = glm::vec4(m_CameraPosition, 1);
	m_CameraBuffer->SetData(&camData);

	// update model
	const float scale = 0.2f;
	glm::mat4 scalemat = glm::scale(glm::mat4(1.0f), { scale, scale, scale });

	glm::mat4 transform = glm::translate(glm::mat4(1.0f), { 0.0f, 0.0f, 0.0f }) * scalemat;
	m_ModelBuffer->SetData(&transform);

	// render
	Engine::RendererCommand::ClearSwapChain(m_NativeWindow.GetSwapChain(), { 1,0,0 });
	Engine::RendererCommand::SetFrameBuffer(m_FrameBuffer);
	Engine::RendererCommand::ClearRenderTarget(m_FrameBuffer->GetRenderTargets()[0], {0,1,0,1}); // clear color
	Engine::RendererCommand::ClearRenderTarget(m_FrameBuffer->GetDepthBuffer(), {1,0,0,0}); // clear depth
	Engine::RendererCommand::SetShader(m_Shader);
	Engine::RendererCommand::SetConstantBuffer(m_Shader->GetBindPoint("Camera"), m_CameraBuffer);
	Engine::RendererCommand::SetConstantBuffer(m_Shader->GetBindPoint("Model"), m_ModelBuffer);
	for (uint32_t i = 0; i < m_Model->GetNumberOfNodes(); i++)
	{
		Engine::RendererCommand::SetTexture(m_Shader->GetBindPoint("texDef"), m_Model->GetNode(i).m_Material->m_Diffuse);
		Engine::RendererCommand::DrawMesh(m_Model->GetNode(i).m_Mesh);
	}

	Engine::RendererCommand::BlitToSwapChain(m_NativeWindow.GetSwapChain(), m_FrameBuffer->GetRenderTargets()[0]);

	DBOUT(Time::GetFPS());
}

void MainWindow::OnClose()
{
}