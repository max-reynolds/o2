#include "o2/stdafx.h"

#ifdef PLATFORM_LINUX

#include "o2/Render/Texture.h"
#include "o2/Utils/Debug/Log/LogStream.h"

namespace o2
{
Texture::~Texture()
{
}

void
Texture::Create(
    const Vec2I &size,
    PixelFormat format /*= Format::Default*/,
    Usage usage /*= Usage::Default*/)
{
}

void
Texture::Create(Bitmap *bitmap)
{
}

void
Texture::SetData(Bitmap *bitmap)
{
}

void
Texture::SetSubData(const Vec2I &offset, Bitmap *bitmap)
{
}

void
Texture::Copy(const Texture &from, const RectI &rect)
{
}

Bitmap *
Texture::GetData()
{
    Bitmap *bitmap = mnew Bitmap(mFormat, mSize);
    return bitmap;
}

void
Texture::SetFilter(Filter filter)
{
}

Texture::Filter
Texture::GetFilter() const
{
    return {};
}
}

#endif