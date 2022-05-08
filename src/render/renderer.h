#ifndef RENDERER_H
#define RENDERER_H

#include "shader.hpp"
#include "xwindow.h"
#include "../infrastructure/communication/ContextHub.h"
#include "../infrastructure/communication/AutoBus.hpp"

namespace RenderSpace {
    class Renderer {
    public:
        Renderer(unsigned int width = 800, unsigned int height = 600);
        ~Renderer();

        int exec();
    
    private:
        void setup();

    private:
        unsigned int m_vao;
        unsigned int m_vbo;
        Shader m_shader;
        RenderWindowWidget m_win_widget;
        GLFWwindow* m_window;

        std::unique_ptr<fundamental::AutoBus> m_autobus;
    };
}

#endif
