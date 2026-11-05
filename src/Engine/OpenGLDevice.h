#pragma once

#include <gabgl/GraphicsDevice.h>
#include <memory>

namespace GAB
{
std::unique_ptr<GraphicsDevice> CreateOpenGLDevice();
}
