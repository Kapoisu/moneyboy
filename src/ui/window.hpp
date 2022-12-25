#ifndef UI_WINDOW_H
#define UI_WINDOW_H

#include <memory>
#include "SDL.h"

namespace gameboy::ui {
    using WindowPtr = std::unique_ptr<SDL_Window, void(*)(SDL_Window*)>;
    using RendererPtr = std::unique_ptr<SDL_Renderer, void(*)(SDL_Renderer*)>;
    using TexturePtr = std::unique_ptr<SDL_Texture, void(*)(SDL_Texture*)>;

    class Window {
    public:
        Window()
        {
            if (SDL_Init(SDL_INIT_VIDEO) < 0) {
                throw std::runtime_error{SDL_GetError()};
            }
        }
        ~Window()
        {
            SDL_Quit();
        }
    };

    struct Width {
        int value;
    };

    struct Height {
        int value;
    };

    struct Scale {
        float value;
    };

    WindowPtr create_window(std::string_view title, Width width, Height height);
    RendererPtr create_renderer(WindowPtr& window, Scale horizontal, Scale vertical);
    TexturePtr create_texture(RendererPtr& renderer, Width width, Height height);
}

#endif