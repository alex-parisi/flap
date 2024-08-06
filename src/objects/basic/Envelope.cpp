/// Envelope.cpp

#include "Envelope.h"

#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui-knobs.h"

#include "../../ConnectionService.h"

void flap::Envelope::initialize() {
    auto envelope = dibiff::dynamic::Envelope::create(_attackTime, _decayTime, _sustainLevel, _releaseTime, _sampleRate);
    _audioObjects.push_back(envelope);
}

void flap::Envelope::render() {
    ImGui::Begin("Envelope");
    // Debug prints to check initial values
    ImGui::Text("Attack: %f", _attackTime);
    ImGui::Text("Decay: %f", _decayTime);
    ImGui::Text("Sustain: %f", _sustainLevel);
    ImGui::Text("Release: %f", _releaseTime);
    if (ImGui::RadioButton("MIDI Input", false)) {
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
    ImGui::SameLine();
    if (ImGui::RadioButton("Input", false)) {
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
    ImGuiKnobs::Knob("Attack", &_attackTime, 0.0f, 2.0f, 0.005f, "%1.3fs", ImGuiKnobVariant_Wiper);
    ImGui::SameLine();
    ImGuiKnobs::Knob("Decay", &_decayTime, 0.0f, 2.0f, 0.005f, "%1.3fs", ImGuiKnobVariant_Wiper);
    ImGui::SameLine();
    ImGuiKnobs::Knob("Sustain", &_sustainLevel, 0.0f, 1.0f, 0.005f, "%1.3f", ImGuiKnobVariant_Wiper);
    ImGui::SameLine();
    ImGuiKnobs::Knob("Release", &_releaseTime, 0.0f, 5.0f, 0.01f, "%1.2fs", ImGuiKnobVariant_Wiper);

    ImGui::End();
}