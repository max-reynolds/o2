#include "o2/stdafx.h"

#ifdef PLATFORM_LINUX

#include "o2/Render/Render.h"

#include "o2/Application/Application.h"
#include "o2/Application/Input.h"
#include "o2/Assets/Assets.h"
#include "o2/Events/EventSystem.h"
#include "o2/Render/Font.h"
#include "o2/Render/Mesh.h"
#include "o2/Render/Sprite.h"
#include "o2/Render/Texture.h"
#include "o2/Utils/Debug/Debug.h"
#include "o2/Utils/Debug/Log/LogStream.h"
#include "o2/Utils/Math/Geometry.h"
#include "o2/Utils/Math/Interpolation.h"

namespace o2
{
Render::Render()
{
}

Render::~Render()
{
}

void
Render::EnableStencilTest()
{
}

void
Render::DisableStencilTest()
{
}

void
Render::ClearStencil()
{
}

void
Render::EnableScissorTest(const RectI &rect)
{
}

void
Render::DisableScissorTest(bool forcible /*= false*/)
{
}

void
Render::BindRenderTexture(TextureRef renderTarget)
{
}

void
Render::DrawBuffer(
    PrimitiveType primitiveType,
    Vertex2 *vertices,
    UInt verticesCount,
    UInt16 *indexes,
    UInt elementsCount,
    const TextureRef &texture)
{
}

void
Render::Begin()
{
}

void
Render::End()
{
}

void
Render::UpdateCameraTransforms()
{
}

void
Render::UnbindRenderTexture()
{
}

void
Render::Clear(const Color4 &color)
{
}
}

#endif
