/// AudioOut.cpp

#include "AudioOut.h"

#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

void flap::AudioOut::initialize() {
    auto graphSink = dibiff::sink::GraphSink::create(_channels, _sampleRate, _blockSize);
    _audioObjects.push_back(graphSink);
}

void flap::AudioOut::render() {
    ImGui::Begin("AudioOut");
    if (_channels == 1) {
        ImGui::RadioButton("Input", false);
    } else {
        ImGui::RadioButton("Input L", false);
        ImGui::SameLine();
        ImGui::RadioButton("Input R", false);
    }
    ImGui::End();
}