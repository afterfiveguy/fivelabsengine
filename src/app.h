#pragma once

#include "window.h"
#include "config.h"
namespace fivelabsengine
{
  class App
  {
  public:
    void run();

  private:
    Window window{WIDTH, HEIGHT, "Fivelabs OpenGL engine"};
  };
} // namespace fivelabsengine
