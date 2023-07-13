#include "Core/Core.h"
#include "Core/Time.h"
#include "MainWindow.h"
#include "Application.h"

#include "Renderer/RendererCommand.h"

struct CameraData
{
	glm::mat4 view;
	glm::mat4 viewPorjection;
};

void MainWindow::OnCreate()
{
	m_NativeWindow.GetSwapChain().SetVSync(false);

	Engine::ShaderSource src;
	src.VetexShader = L"VertexShader.vertex.cso";
	src.PixelShader = L"PixelShader.pixel.cso";
	m_Shader = Engine::Shader::Create(src);

	m_Camera = Engine::Camera::Create(Engine::Camera::ProjectionType::Perspective, glm::radians(45.0f), 0.01f, 100.0f, GetAspect());
	m_CameraBuffer = Engine::ConstantBuffer::Create(sizeof(CameraData));

	m_Mesh = Engine::Mesh::Create("Assets/Models/nanosuit.obj");
	m_ModelBuffer = Engine::ConstantBuffer::Create(sizeof(glm::mat4));
}

float rot = 0.0f;
void MainWindow::OnUpdate()
{
	// handel input
	float rotSpeed = 1.0f;
	const float deltaTime = Time::GetDeltaTime();
	if (m_Input.GetKeyDown(VK_LEFT))
		rot += rotSpeed * deltaTime;
	if (m_Input.GetKeyDown(VK_RIGHT))
		rot -= rotSpeed * deltaTime;

	glm::mat4 camRot = glm::rotate(glm::mat4(1.0f), rot, { 0.0f, 1.0f, 0.0f });

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
	m_CameraBuffer->SetData(&camData);

	// update model
	const float scale = 0.2f;
	glm::mat4 scalemat = glm::scale(glm::mat4(1.0f), { scale, scale, scale });

	glm::mat4 transform = glm::translate(glm::mat4(1.0f), { 0.0f, -1.5f, 0.0f }) * scalemat;
	m_ModelBuffer->SetData(&transform);

	// render
	Engine::RendererCommand::SetSwapChain(m_NativeWindow.GetSwapChain());
	ClearToColor(1.0f, 0.0f, 0.0f); // TODO : fix
	Engine::RendererCommand::SetShader(m_Shader);
	Engine::RendererCommand::SetConstantBuffer(m_Shader->GetBindPoint("Camera"), m_CameraBuffer);
	Engine::RendererCommand::SetConstantBuffer(m_Shader->GetBindPoint("Model"), m_ModelBuffer);
	for (uint32_t c = 0; c < m_Mesh->m_Children.size(); c++)
	{
		Engine::RendererCommand::DrawMesh(m_Mesh->m_Children[c]->m_Meshes[0]);
	}
}

void MainWindow::OnClose()
{
	Engine::Application::Quit(69);
}