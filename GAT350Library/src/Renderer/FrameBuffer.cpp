#include "FrameBuffer.h"

#include "RenderTarget.h"

namespace Engine
{

	FrameBuffer::FrameBuffer(const std::vector<Ref<RenderTarget>>& targets)
	{
		for (Ref<RenderTarget> target : targets)
		{
			if (target->IsDepthStencilTexture())
			{
				if (m_DepthBuffer == nullptr)
					m_DepthBuffer = target;
				else
					DBOUT("Frame Buffer already has a depth texture");
			}
			else
				m_RenderTargets.push_back(target);
		}
	}

	Ref<FrameBuffer> FrameBuffer::Create(const std::vector<Ref<RenderTarget>>& targets)
	{
		return std::make_shared<FrameBuffer>(targets);
	}

}

