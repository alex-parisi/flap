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
    } else {
        if (ImGui::RadioButton("Input L", _inputLSelected)) {
            if (!_inputLSelected) {
                if (!ConnectionService::getInstance().isDragging()) {
                    ConnectionService::getInstance().startDragging(_getRadioButtonCenter(), _audioObjects[0]->getInput(0), _audioObjects[0], _inputLSelected);
                } else {
                    ConnectionService::getInstance().stopDragging(_getRadioButtonCenter(), _audioObjects[0]->getInput(0), _audioObjects[0]);
                    _inputLSelected = true;
                }
            } else {
                /// TODO: Disconnect
            }
        }
        ImGui::SameLine();
        if (ImGui::RadioButton("Input R", _inputRSelected)) {
            if (!_inputRSelected) {
                if (!ConnectionService::getInstance().isDragging()) {
                    ConnectionService::getInstance().startDragging(_getRadioButtonCenter(), _audioObjects[0]->getInput(1), _audioObjects[0], _inputRSelected);
                } else {
                    ConnectionService::getInstance().stopDragging(_getRadioButtonCenter(), _audioObjects[0]->getInput(1), _audioObjects[0]);
                    _inputRSelected = true;
                }
            } else {
                /// TODO: Disconnect
            }
        }
    }
    ImGui::End();
}