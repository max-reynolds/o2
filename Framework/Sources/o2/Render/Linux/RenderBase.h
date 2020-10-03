#pragma once

#ifdef PLATFORM_LINUX

#include <cstdint>

namespace o2
{
    class RenderBase
    {
    protected:
        uint32_t    mIndexBufferSize;                // Maximum size of index buffer
    };
}

#endif
