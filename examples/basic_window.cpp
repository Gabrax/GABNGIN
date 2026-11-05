#include <gabgl/gabgl.h>

int main()
{
  if (!GAB::INIT()) return 1;
  float x = 100.0f;
  while (!GAB::WindowShouldClose())
  {
    if (!GAB::BeginDrawing()) continue;
    if (GAB::IsKeyDown(GAB::Key::D)) x += 200.0f * GAB::GetFrameTime();
    if (GAB::IsKeyDown(GAB::Key::A)) x -= 200.0f * GAB::GetFrameTime();
    GAB::ClearBackground({0.04f, 0.06f, 0.1f, 1.0f});
    GAB::DrawRectangle(x, 180, 90, 90, {0.2f, 0.7f, 1.0f, 1.0f});
    GAB::DrawLine({40, 320}, {760, 320}, 2, {0.5f, 0.5f, 0.5f, 1});
    GAB::EndDrawing();
  }
  GAB::TERMINATE();
}
