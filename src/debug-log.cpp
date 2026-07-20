#include "debug-log.h"

namespace engineDebugger {

void DebugConsole::log(const std::string &message) {
  lines.push_back(message);
  if (lines.size() > maxLines)
    lines.erase(lines.begin());
};

void DebugConsole::render(SDL_Renderer *renderer, TTF_Font *font) const {
  SDL_Color white{255, 255, 255};
  float y = 10.0f;
  for (const std::string &line : lines) {
    SDL_Surface *textSurface =
        TTF_RenderText_Blended(font, line.c_str(), 0, white);
    SDL_Texture *textTexture =
        SDL_CreateTextureFromSurface(renderer, textSurface);
    SDL_DestroySurface(
        textSurface); // pixels are on the GPU now, drop the CPU copy here
    float textW = 0, textH = 0;
    SDL_GetTextureSize(textTexture, &textW, &textH);
    SDL_FRect dst{10.0f, y, textW, textH};
    SDL_RenderTexture(renderer, textTexture, nullptr, &dst);

    SDL_DestroyTexture(textTexture);

    y += textH;
  }
};
} // namespace engineDebugger
