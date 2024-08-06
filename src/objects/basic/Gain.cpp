/// Gain.cpp

#include "Gain.h"

#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "../../ConnectionService.h"

void flap::Gain::initialize() {
    _gain = std::make_shared<float>(0.0f);
    auto gainObject = dibiff::level::Gain::create(_gain);
    _audioObjects.push_back(gainObject);
}

void flap::Gain::render() {
    ImGui::Begin("Gain");
    if (ImGui::RadioButton("Input", false)) {
        if (!ConnectionService::getInstance().isDragging()) {
            ImVec2 mousePos = ImGui::GetMousePos();
            ConnectionService::getInstance().addPoint(mousePos);
            ConnectionService::getInstance().setDragging(true);
            ConnectionService::getInstance().setActivePoint(_audioObjects[0]->getInput());
            ConnectionService::getInstance().setActiveObject(_audioObjects[0]);
        } else {
            ConnectionService::getInstance().setDragging(false);
            /// Ensure we're not connecting this to itself
            if (ConnectionService::getInstance().getActiveObject() != _audioObjects[0]) {
                /// Try to connect the active point to the input of the gain object
                try {
                    std::lock_guard<std::mutex> lock(*ConnectionService::getInstance().getMutex());
                    dibiff::graph::AudioGraph::connect(ConnectionService::getInstance().getActivePoint(), _audioObjects[0]->getInput());
                    ConnectionService::getInstance().removePoint();
                    std::cout << "Connected" << std::endl;
                } catch (std::exception& e) {
                    /// If we can't connect, remove the last point
                    ConnectionService::getInstance().removePoint();
                }
            } else {
                std::cout << "Can't connect to itself" << std::endl;
                ConnectionService::getInstance().removePoint();
            }
        }
    }
    ImGui::SameLine();
    if (ImGui::RadioButton("Output", false)) {
        if (!ConnectionService::getInstance().isDragging()) {
            ImVec2 mousePos = ImGui::GetMousePos();
            ConnectionService::getInstance().addPoint(mousePos);
            ConnectionService::getInstance().setDragging(true);
            ConnectionService::getInstance().setActivePoint(_audioObjects[0]->getOutput());
            ConnectionService::getInstance().setActiveObject(_audioObjects[0]);
        } else {
            ConnectionService::getInstance().setDragging(false);
            /// Ensure we're not connecting this to itself
            if (ConnectionService::getInstance().getActiveObject() != _audioObjects[0]) {
                /// Try to connect the active point to the input of the gain object
                try {
                    std::lock_guard<std::mutex> lock(*ConnectionService::getInstance().getMutex());
                    dibiff::graph::AudioGraph::connect(ConnectionService::getInstance().getActivePoint(), _audioObjects[0]->getOutput());
                    ConnectionService::getInstance().removePoint();
                    std::cout << "Connected" << std::endl;
                } catch (std::exception& e) {
                    /// If we can't connect, remove the last point
                    ConnectionService::getInstance().removePoint();
                }
            } else {
                std::cout << "Can't connect to itself" << std::endl;
                ConnectionService::getInstance().removePoint();
            }
        }
    }
    ImGui::End();
}