/// Gain.cpp

#include "Gain.h"

#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui-knobs.h"

#include "../../ConnectionService.h"

void flap::Gain::initialize() {
    auto gainObject = dibiff::level::Gain::create(_gain);
    _audioObjects.push_back(gainObject);
}

void flap::Gain::render() {
    ImGui::Begin("Gain");
    if (ImGui::RadioButton("Input", _inputSelected)) {
        if (!_inputSelected) {
            if (!ConnectionService::getInstance().isDragging()) {
                ConnectionService::getInstance().startDragging(_getRadioButtonCenter(), _audioObjects[0]->getInput(), _audioObjects[0], _inputSelected);
            } else {
                ConnectionService::getInstance().stopDragging(_getRadioButtonCenter(), _audioObjects[0]->getInput(), _audioObjects[0]);
                _inputSelected = true;
            }
        } else {
            /// TODO: Disconnect
        }
    }
    ImGui::SameLine();
    if (ImGui::RadioButton("Output", _outputSelected)) {
        if (!ConnectionService::getInstance().isDragging()) {
            ConnectionService::getInstance().startDragging(_getRadioButtonCenter(), _audioObjects[0]->getOutput(), _audioObjects[0], _outputSelected);
        } else {
            ConnectionService::getInstance().stopDragging(_getRadioButtonCenter(), _audioObjects[0]->getOutput(), _audioObjects[0]);
            _outputSelected = true;
        }
    }
    ImGuiKnobs::Knob("Gain", &_gain, -80.0f, 30.0f, 0.1f, "%.1fdB", ImGuiKnobVariant_Wiper);
    ImGui::End();
}