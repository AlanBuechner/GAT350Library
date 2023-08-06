#pragma once
#include "Platform/Windows/Win.h"
#include "Core/Core.h"

#include <vector>

namespace Engine
{
	class RenderTarget;
}

namespace Engine
{
	class FrameBuffer
	{
	public:

		FrameBuffer(const std::vector<Ref<RenderTarget>>& targets);

		const std::vector<Ref<RenderTarget>>& GetRenderTargets() { return m_RenderTargets; }
		Ref<RenderTarget> GetDepthBuffer() { return m_DepthBuffer; }
		bool HasDepthBuffer() { return m_DepthBuffer != nullptr; }

		static Ref<FrameBuffer> Create(const std::vector<Ref<RenderTarget>>& targets);

	private:
		std::vector<Ref<RenderTarget>> m_RenderTargets;
		Ref<RenderTarget> m_DepthBuffer;

	};
}