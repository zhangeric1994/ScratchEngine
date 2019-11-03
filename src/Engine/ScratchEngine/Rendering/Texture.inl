#ifndef TEXTURE_INL
#define TEXTURE_INL
#pragma once

#include "prerequisites.h"


inline u32 ScratchEngine::Rendering::Texture2D::GetWidth() const
{
	return width;
}

inline u32 ScratchEngine::Rendering::Texture2D::GetHeight() const
{
	return height;
}


inline u32 ScratchEngine::Rendering::TextureCube::GetSize() const
{
	return size;
}


inline LightType ScratchEngine::Rendering::Shadow::GetType() const
{
	return type;
}

inline u32 ScratchEngine::Rendering::Shadow::GetWidth() const
{
	return width;
}

inline u32 ScratchEngine::Rendering::Shadow::GetHeight() const
{
	return height;
}

inline i32 ScratchEngine::Rendering::Shadow::GetNumCascades() const
{
	return numCascades;
}

#endif // !TEXTURE_INL