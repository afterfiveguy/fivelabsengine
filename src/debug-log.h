#pragma once

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <string>
#include <vector>

namespace engineDebugger {

class DebugConsole {
public:
  void log(const std::string &message);
  void render(SDL_Renderer *renderer, TTF_Font *font) const;

private:
  std::vector<std::string> lines;
  size_t maxLines = 12;
};
} // namespace engineDebugger
