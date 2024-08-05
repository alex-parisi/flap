/// MidiIn.cpp

#include "MidiIn.h"

#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

void flap::MidiIn::initialize() {
    auto midiObject = dibiff::midi::MidiInput::create(_blockSize);
    _audioObjects.push_back(midiObject);
}

void flap::MidiIn::render() {
    ImGui::Begin("MidiInput");
    ImGui::RadioButton("Output", false);
    ImGui::End();
}