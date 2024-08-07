/// MainApplication.h

#pragma once

#include <iostream>
#include <condition_variable>
#include <mutex>
#include <vector>
#include <memory>

#include "glad.h"
#include <GLFW/glfw3.h>

#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "managers/AudioManager.h"
#include "managers/GraphManager.h"
#include "managers/MidiManager.h"
#include "objects/Object.h"

#include "MainApplicationSettings.h"

namespace flap {
    class MainApplication {
        public:
            MainApplication(std::shared_ptr<MainApplicationSettings> settings) : _settings(settings), _audioManager(settings), _graphManager(settings), _midiManager(settings) {};
            bool initialize();
            void run();
            void cleanup();
        private:
            AudioManager _audioManager;
            GraphManager _graphManager;
            MidiManager _midiManager;

            /// TODO: Use a smart pointer here
            GLFWwindow* _window;

            double _lastRenderTime = 0.0;
            const double _renderFPS = 60.0;
            const double _renderInterval = 1.0 / _renderFPS;
            std::vector<std::shared_ptr<Object>> _objects;
            std::shared_ptr<MainApplicationSettings> _settings;
            static void _glfwErrorCallback(int error, const char* description) {
                std::cerr << "GLFW Error " << error << ": " << description << "\n";
            }
            void _render();
            void _renderImGui(int screenWidth, int screenHeight);
            void _renderToolbar();
    };
}