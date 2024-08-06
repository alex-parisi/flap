/// SineGenerator.cpp

#include "SineGenerator.h"

#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "../../ConnectionService.h"

void flap::SineGenerator::initialize() {
    auto sineObject = dibiff::generator::SineGenerator::create(_settings->blockSize, _settings->sampleRate);
    _audioObjects.push_back(sineObject);
}

void flap::SineGenerator::render() {
    ImGui::Begin("Sine Generator");
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
    ImGui::End();
}