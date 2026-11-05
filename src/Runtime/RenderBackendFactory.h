#pragma once

#include <memory>

namespace GAB
{
class IRenderBackend;

std::unique_ptr<IRenderBackend> CreateOpenGLRenderBackend();
std::unique_ptr<IRenderBackend> CreateDirectX12RenderBackend();
std::unique_ptr<IRenderBackend> CreateVulkanRenderBackend();
} // namespace GAB
