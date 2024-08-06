/// SineGenerator.cpp

#include "SineGenerator.h"

#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

void flap::SineGenerator::initialize() {
    auto sineObject = dibiff::generator::SineGenerator::create(_settings->blockSize, _settings->sampleRate);
    _audioObjects.push_back(sineObject);
}

void flap::SineGenerator::render() {
    ImGui::Begin("Sine Generator");
    ImGui::RadioButton("Input", false);
    ImGui::SameLine();
    ImGui::RadioButton("Output", false);
    ImGui::End();
}