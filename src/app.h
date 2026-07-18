#pragma once

#include "window.h"

namespace fivelabsengine {
class App {
public:
  static constexpr int WIDTH = 800;
  static constexpr int HEIGHT = 600;

  void run();

private:
  Window window{WIDTH, HEIGHT, "Fivelabs OpenGL engine"};
};
} // namespace fivelabsengine
