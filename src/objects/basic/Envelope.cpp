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
    if (ImGui::RadioButton("MIDI Input", _midiInputSelected)) {
        if (!_midiInputSelected) {
            if (!ConnectionService::getInstance().isDragging()) {
                ConnectionService::getInstance().startDragging(_getRadioButtonCenter(), _audioObjects[0]->getInput(1), _audioObjects[0], _midiInputSelected);
            } else {
                ConnectionService::getInstance().stopDragging(_getRadioButtonCenter(), _audioObjects[0]->getInput(1), _audioObjects[0]);
                _midiInputSelected = true;
            }
        } else {
            /// TODO: Disconnect 
        }
    }
    ImGui::SameLine();
    if (ImGui::RadioButton("Input", _inputSelected)) {
        if (!_inputSelected) {
            if (!ConnectionService::getInstance().isDragging()) {
                ConnectionService::getInstance().startDragging(_getRadioButtonCenter(), _audioObjects[0]->getInput(0), _audioObjects[0], _inputSelected);
            } else {
                ConnectionService::getInstance().stopDragging(_getRadioButtonCenter(), _audioObjects[0]->getInput(0), _audioObjects[0]);
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
    ImGuiKnobs::Knob("Attack", &_attackTime, 0.0f, 2.0f, 0.005f, "%1.3fs", ImGuiKnobVariant_Wiper);
    ImGui::SameLine();
    ImGuiKnobs::Knob("Decay", &_decayTime, 0.0f, 2.0f, 0.005f, "%1.3fs", ImGuiKnobVariant_Wiper);
    ImGui::SameLine();
    ImGuiKnobs::Knob("Sustain", &_sustainLevel, 0.0f, 1.0f, 0.005f, "%1.3f", ImGuiKnobVariant_Wiper);
    ImGui::SameLine();
    ImGuiKnobs::Knob("Release", &_releaseTime, 0.0f, 5.0f, 0.01f, "%1.2fs", ImGuiKnobVariant_Wiper);

    ImGui::End();
}