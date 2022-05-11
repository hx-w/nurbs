﻿#ifndef RENDERER_H
#define RENDERER_H

#include "service.h"
#include "xwindow.h"

namespace RenderSpace {
    class RenderEngine;

    class Renderer {
    public:
        Renderer(RenderEngine& engine, unsigned int width, unsigned int height);
        ~Renderer();

        int exec();
    
    private:
        void setup();
        void update_transform();
        void draw_vertex();
        void draw_mesh();

    private:
        // Shader m_shader;
        RenderWindowWidget m_win_widget;
        GLFWwindow* m_window;
    
    private:
        RenderEngine& m_engine;
        std::unique_ptr<RenderService> m_service;
    };
}

#endif
