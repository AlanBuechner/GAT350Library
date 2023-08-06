#pragma once
#include "Core/Core.h"
#include "Texture.h"

namespace Engine
{
	struct Material
	{

		Ref<Texture2D> m_Diffuse;
		Ref<Texture2D> m_Normal;
		Ref<Texture2D> m_Roughness;
		Ref<Texture2D> m_Metal;
		Ref<Texture2D> m_AO;

		static Ref<Material> Create() {
			return std::make_shared<Material>();
		}

	};
}