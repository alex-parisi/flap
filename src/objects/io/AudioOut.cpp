/// AudioOut.cpp

#include "AudioOut.h"

#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "../../ConnectionService.h"

void flap::AudioOut::initialize() {
    auto graphSink = dibiff::sink::GraphSink::create(_channels, _sampleRate, _blockSize);
    _audioObjects.push_back(graphSink);
}

void flap::AudioOut::render() {
    ImGui::Begin("AudioOut");
    if (_channels == 1) {
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
    } else {
        if (ImGui::RadioButton("Input L", false)) {
            if (!ConnectionService::getInstance().isDragging()) {
                ImVec2 mousePos = ImGui::GetMousePos();
                ConnectionService::getInstance().addPoint(mousePos);
                ConnectionService::getInstance().setDragging(true);
                ConnectionService::getInstance().setActivePoint(_audioObjects[0]->getInput(0));
                ConnectionService::getInstance().setActiveObject(_audioObjects[0]);
            } else {
                ConnectionService::getInstance().setDragging(false);
                /// Ensure we're not connecting this to itself
                if (ConnectionService::getInstance().getActiveObject() != _audioObjects[0]) {
                    /// Try to connect the active point to the input of the gain object
                    try {
                        std::lock_guard<std::mutex> lock(*ConnectionService::getInstance().getMutex());
                        dibiff::graph::AudioGraph::connect(ConnectionService::getInstance().getActivePoint(), _audioObjects[0]->getInput(0));
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
        if (ImGui::RadioButton("Input R", false)) {
            if (!ConnectionService::getInstance().isDragging()) {
                ImVec2 mousePos = ImGui::GetMousePos();
                ConnectionService::getInstance().addPoint(mousePos);
                ConnectionService::getInstance().setDragging(true);
                ConnectionService::getInstance().setActivePoint(_audioObjects[0]->getInput(1));
                ConnectionService::getInstance().setActiveObject(_audioObjects[0]);
            } else {
                ConnectionService::getInstance().setDragging(false);
                /// Ensure we're not connecting this to itself
                if (ConnectionService::getInstance().getActiveObject() != _audioObjects[0]) {
                    /// Try to connect the active point to the input of the gain object
                    try {
                        std::lock_guard<std::mutex> lock(*ConnectionService::getInstance().getMutex());
                        dibiff::graph::AudioGraph::connect(ConnectionService::getInstance().getActivePoint(), _audioObjects[0]->getInput(1));
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
    }
    ImGui::End();
}