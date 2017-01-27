#include "Render/Render.h"

namespace o2
{
	Render::Render()
    {
        Initialize();
    }

	Render::~Render()
	{
        Deinitialize();
    }
}
