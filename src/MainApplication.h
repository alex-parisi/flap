/// MainApplication.h

#pragma once

#include <iostream>
#include <condition_variable>
#include <mutex>

#include "glad.h"
#include <GLFW/glfw3.h>

#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "managers/AudioManager.h"
#include "managers/GraphManager.h"
#include "managers/MidiManager.h"

class MainApplication {
public:
    bool initialize();
    void run();
    void cleanup();
private:
    AudioManager _audioManager;
    GraphManager _graphManager;
    MidiManager _midiManager;
    GLFWwindow* _window;
    double _lastRenderTime;
    const double _renderFPS = 60.0;
    const double _renderInterval = 1.0 / _renderFPS;
    static void _glfwErrorCallback(int error, const char* description) {
        std::cerr << "GLFW Error " << error << ": " << description << "\n";
    }
    void _render();
};