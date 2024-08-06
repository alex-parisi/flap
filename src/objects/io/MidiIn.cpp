/// MidiIn.cpp

#include "MidiIn.h"

#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "../../ConnectionService.h"

void flap::MidiIn::initialize() {
    auto midiObject = dibiff::midi::MidiInput::create(_blockSize);
    _audioObjects.push_back(midiObject);
}

void flap::MidiIn::render() {
    ImGui::Begin("MidiInput");
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