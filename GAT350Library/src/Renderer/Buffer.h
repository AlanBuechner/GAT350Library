#pragma once
#include "Platform/Windows/Win.h"
#include "Core/Core.h"

namespace Engine
{

#pragma region buffer layout

	enum class ShaderDataType
	{
		None,
		Float, Float2, Float3, Float4,
		Mat3, Mat4,
		Int, Int2, Int3, Int4,
		Bool
	};

	static uint32_t ShaderDataTypeSize(ShaderDataType type)
	{
		switch (type)
		{
		case Engine::ShaderDataType::Float:		return 4;
		case Engine::ShaderDataType::Float2:	return 4 * 2;
		case Engine::ShaderDataType::Float3:	return 4 * 3;
		case Engine::ShaderDataType::Float4:	return 4 * 4;
		case Engine::ShaderDataType::Mat3:		return 4 * 3 * 3;
		case Engine::ShaderDataType::Mat4:		return 4 * 4 * 4;
		case Engine::ShaderDataType::Int:		return 4;
		case Engine::ShaderDataType::Int2:		return 4 * 2;
		case Engine::ShaderDataType::Int3:		return 4 * 3;
		case Engine::ShaderDataType::Int4:		return 4 * 4;
		case Engine::ShaderDataType::Bool:		return 1;
		}
		return 0;
	}

#pragma endregion

	class VertexBuffer
	{
	public:
		VertexBuffer(const void* vertices, uint32_t stride, uint32_t count);

		void SetData(const void* data, uint32_t count);

		uint32_t GetStride() { return m_Stride; }
		uint32_t GetCount() { return m_Count; }
		wrl::ComPtr<ID3D11Buffer> GetBuffer() { return m_Buffer; }

		static Ref<VertexBuffer> Create(uint32_t stride, uint32_t count);
		static Ref<VertexBuffer> Create(const void* vertices, uint32_t stride, uint32_t count);

	private:
		wrl::ComPtr<ID3D11Buffer> m_Buffer;
		uint32_t m_Stride;
		uint32_t m_Count;
	};



	class IndexBuffer
	{
	public:
		IndexBuffer(const uint32_t* indices, uint32_t count);

		void SetData(const uint32_t* indices, uint32_t count);

		uint32_t GetCount() const { return m_Count; };
		wrl::ComPtr<ID3D11Buffer> GetBuffer() { return m_Buffer; }
		
		static Ref<IndexBuffer> Create(const uint32_t count);
		static Ref<IndexBuffer> Create(const uint32_t* indices, uint32_t count);

	private:
		wrl::ComPtr<ID3D11Buffer> m_Buffer;
		uint32_t m_Count;
	};



	class ConstantBuffer
	{
	public:
		ConstantBuffer(const void* data, uint32_t size);

		void SetData(const void* data);

		uint32_t GetSize() { return m_Size; }
		wrl::ComPtr<ID3D11Buffer> GetBuffer() { return m_Buffer; }

		static Ref<ConstantBuffer> Create(uint32_t size);
		static Ref<ConstantBuffer> Create(const void* data, uint32_t size);

	private:
		wrl::ComPtr<ID3D11Buffer> m_Buffer;
		uint32_t m_Size;
	};



	class StructuredBuffer
	{
	public:
		StructuredBuffer(uint32_t stride, uint32_t count);

		void SetData(const void* data);

		uint32_t GetStride() { return m_Stride; }
		uint32_t GetCount() { return m_Count; }
		const wrl::ComPtr<ID3D11Buffer> GetBuffer() const { return m_Buffer; }
		const wrl::ComPtr<ID3D11ShaderResourceView> GetSRV() const { return m_SRV; }

		static Ref<StructuredBuffer> Create(uint32_t stride, uint32_t count);

	private:

	private:
		wrl::ComPtr<ID3D11Buffer> m_Buffer;
		wrl::ComPtr<ID3D11ShaderResourceView> m_SRV;

		uint32_t m_Stride;
		uint32_t m_Count;

	};
}
