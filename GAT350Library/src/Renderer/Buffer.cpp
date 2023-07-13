#include "Buffer.h"
#include "RendererAPI.h"

namespace Engine
{

	DXGI_FORMAT ShaderDataTypeToDirectXFormat(ShaderDataType type)
	{
		switch (type)
		{
		case Engine::ShaderDataType::None:		return DXGI_FORMAT_UNKNOWN;
		case Engine::ShaderDataType::Float:		return DXGI_FORMAT_R32_FLOAT;
		case Engine::ShaderDataType::Float2:	return DXGI_FORMAT_R32G32_FLOAT;
		case Engine::ShaderDataType::Float3:	return DXGI_FORMAT_R32G32B32_FLOAT;
		case Engine::ShaderDataType::Float4:	return DXGI_FORMAT_R32G32B32A32_FLOAT;
		case Engine::ShaderDataType::Int:		return DXGI_FORMAT_R32_SINT;
		case Engine::ShaderDataType::Int2:		return DXGI_FORMAT_R32G32_SINT;
		case Engine::ShaderDataType::Int3:		return DXGI_FORMAT_R32G32B32_SINT;
		case Engine::ShaderDataType::Int4:		return DXGI_FORMAT_R32G32B32A32_SINT;
		case Engine::ShaderDataType::Mat3:
			break;
		case Engine::ShaderDataType::Mat4:
			break;
		case Engine::ShaderDataType::Bool:
			break;
		}
		return DXGI_FORMAT_UNKNOWN;
	}

	// ------------------------------------- Vertex Buffer ------------------------------------- //

#pragma region Vertex Buffer

	VertexBuffer::VertexBuffer(const void* vertices, uint32_t stride, uint32_t count) :
		m_Stride(stride), m_Count(count)
	{
		RendererAPI& graphics = RendererAPI::Get();
		D3D11_BUFFER_DESC vbufferDesc = { 0 };
		vbufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vbufferDesc.Usage = (vertices == nullptr ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT);
		vbufferDesc.CPUAccessFlags = (vertices == nullptr ? D3D11_CPU_ACCESS_WRITE : 0u);
		vbufferDesc.MiscFlags = 0u;
		vbufferDesc.ByteWidth = count * stride;

		D3D11_SUBRESOURCE_DATA vbufferData = { 0 };
		if (vertices == nullptr)
			vbufferData.pSysMem = (const void*)new byte[count * stride]{ 0 };
		else
			vbufferData.pSysMem = vertices;

		HRESULT hr = graphics.GetDivice()->CreateBuffer(&vbufferDesc, &vbufferData, &m_Buffer);

		if (FAILED(hr))
			DBOUT("failed to create vertex buffer " << TranslateError(hr) << std::endl);

		if (vertices == nullptr)
			delete[] vbufferData.pSysMem;
	}

	void VertexBuffer::SetData(const void* data, uint32_t count)
	{
		RendererAPI& graphics = RendererAPI::Get();

		D3D11_MAPPED_SUBRESOURCE msub = {};

		HRESULT hr = graphics.GetContext()->Map(m_Buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &msub);

		if (FAILED(hr))
		{
			DBOUT("failed to update vertex buffer " << TranslateError(hr) << std::endl);
			return;
		}

		CopyMemory(msub.pData, data, count * m_Stride);
		graphics.GetContext()->Unmap(m_Buffer.Get(), 0);
	}

	Ref<VertexBuffer> VertexBuffer::Create(uint32_t stride, uint32_t count)
	{
		return Create(nullptr, stride, count);
	}

	Ref<VertexBuffer> VertexBuffer::Create(const void* vertices, uint32_t stride, uint32_t count)
	{
		return std::make_shared<VertexBuffer>(vertices, stride, count);
	}

#pragma endregion

	// ------------------------------------- End Vertex Buffer ------------------------------------- //

	// ------------------------------------- Index Buffer ------------------------------------- //

#pragma region Index Buffer

	IndexBuffer::IndexBuffer(const uint32_t* indices, uint32_t count) :
		m_Count(count)
	{
		RendererAPI& graphics = RendererAPI::Get();
		m_Count = count;

		D3D11_BUFFER_DESC ibufferDesc = { 0 };
		ibufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		ibufferDesc.Usage = (indices == nullptr ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT);
		ibufferDesc.CPUAccessFlags = (indices == nullptr ? D3D11_CPU_ACCESS_WRITE : 0u);
		ibufferDesc.MiscFlags = 0u;
		ibufferDesc.ByteWidth = sizeof(uint32_t) * count;
		ibufferDesc.StructureByteStride = sizeof(uint32_t);

		D3D11_SUBRESOURCE_DATA ibufferData = { 0 };
		if (indices == nullptr)
			ibufferData.pSysMem = (const void*)new uint32_t[count]{ 0 };
		else
			ibufferData.pSysMem = indices;

		graphics.GetDivice()->CreateBuffer(&ibufferDesc, &ibufferData, &m_Buffer);

		if (indices == nullptr)
			delete[] ibufferData.pSysMem;
	}

	void IndexBuffer::SetData(const uint32_t* indices, uint32_t count)
	{
		RendererAPI& graphics = RendererAPI::Get();

		D3D11_MAPPED_SUBRESOURCE msub = {};

		HRESULT hr = graphics.GetContext()->Map(m_Buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &msub);

		if (FAILED(hr))
		{
			DBOUT("failed to update index buffer " << TranslateError(hr) << std::endl);
			return;
		}

		CopyMemory(msub.pData, indices, count * sizeof(uint32_t));
		graphics.GetContext()->Unmap(m_Buffer.Get(), 0);
	}


	Ref<IndexBuffer> IndexBuffer::Create(const uint32_t count)
	{
		return Create(nullptr, count);
	}

	Ref<IndexBuffer> IndexBuffer::Create(const uint32_t* indices, const uint32_t count)
	{
		return std::make_shared<IndexBuffer>(indices, count);
	}

#pragma endregion

	// ------------------------------------- End Index Buffer ------------------------------------- //


	// ------------------------------------- Constant Buffer ------------------------------------- //

#pragma region Constant Buffer

	ConstantBuffer::ConstantBuffer(const void* data, uint32_t size) :
		m_Size(size)
	{
		RendererAPI& graphics = RendererAPI::Get();

		D3D11_BUFFER_DESC cbufferDesc = {  };
		cbufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		cbufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		cbufferDesc.MiscFlags = 0u;
		cbufferDesc.ByteWidth = size;
		cbufferDesc.StructureByteStride = 0u;
		D3D11_SUBRESOURCE_DATA cbufferData = {  };
		if (data == nullptr)
			cbufferData.pSysMem = (const void*) new byte[size]{ 0 };
		else
			cbufferData.pSysMem = data;

		HRESULT hr = graphics.GetDivice()->CreateBuffer(&cbufferDesc, &cbufferData, &m_Buffer);

		if (FAILED(hr))
			DBOUT("faild to create constent buffer " << TranslateError(hr) << std::endl);

		if (data == nullptr)
			delete[] cbufferData.pSysMem;
	}

	void ConstantBuffer::SetData(const void* data)
	{
		RendererAPI& graphics = RendererAPI::Get();

		D3D11_MAPPED_SUBRESOURCE msub = {};

		HRESULT hr = graphics.GetContext()->Map(m_Buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &msub);

		if (FAILED(hr))
		{
			DBOUT("failed to update constent buffer " << TranslateError(hr) << std::endl);
			return;
		}

		CopyMemory(msub.pData, data, m_Size);
		graphics.GetContext()->Unmap(m_Buffer.Get(), 0);
	}

	Ref<ConstantBuffer> ConstantBuffer::ConstantBuffer::Create(uint32_t size)
	{
		return Create(nullptr, size);
	}

	Ref<ConstantBuffer> ConstantBuffer::Create(const void* data, uint32_t size)
	{
		return std::make_shared<ConstantBuffer>(data, size);
	}

#pragma endregion


	// ------------------------------------- End Constant Buffer ------------------------------------- //

	StructuredBuffer::StructuredBuffer(uint32_t stride, uint32_t count) :
		m_Stride(stride), m_Count(count)
	{
		if (count == 0)
			return;

		RendererAPI& graphics = RendererAPI::Get();

		D3D11_BUFFER_DESC sbufferDesc = {  };
		sbufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		sbufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		sbufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		sbufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		sbufferDesc.ByteWidth = count * stride;
		sbufferDesc.StructureByteStride = stride;

		D3D11_SUBRESOURCE_DATA sbufferData = { 0 };
		sbufferData.pSysMem = new uint8_t[count * stride]{ 0 };

		HRESULT hr = graphics.GetDivice()->CreateBuffer(&sbufferDesc, &sbufferData, &m_Buffer);

		if (FAILED(hr))
			DBOUT("failed to create structured buffer " << TranslateError(hr) << std::endl);

		delete[] sbufferData.pSysMem;

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		srvDesc.Format = DXGI_FORMAT_UNKNOWN;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
		srvDesc.Buffer.FirstElement = 0;
		srvDesc.Buffer.NumElements = count;

		hr = graphics.GetDivice()->CreateShaderResourceView(m_Buffer.Get(), &srvDesc, &m_SRV);
		if (FAILED(hr))
			DBOUT("failed to create structured buffer resource view " << TranslateError(hr) << std::endl);
	}

	void StructuredBuffer::SetData(const void* data)
	{
		if (m_Count == 0)
			return;

		RendererAPI& graphics = RendererAPI::Get();

		D3D11_MAPPED_SUBRESOURCE msub = {};

		HRESULT hr = graphics.GetContext()->Map(m_Buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &msub);

		if (FAILED(hr))
		{
			DBOUT("failed to update Structured buffer " << TranslateError(hr) << std::endl);
			return;
		}

		CopyMemory(msub.pData, data, m_Count * m_Stride);
		graphics.GetContext()->Unmap(m_Buffer.Get(), 0);
	}

	Ref<StructuredBuffer> StructuredBuffer::Create(uint32_t stride, uint32_t count)
	{
		return std::make_shared<StructuredBuffer>(stride, count);
	}

}