/// Gain.cpp

#include "Gain.h"

#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

void flap::Gain::initialize() {
    _gain = std::make_shared<float>(0.0f);
    auto gainObject = dibiff::level::Gain::create(_gain);
    _audioObjects.push_back(gainObject);
}

void flap::Gain::render() {
    ImGui::Begin("Gain");
    ImGui::RadioButton("Input", false);
    ImGui::SameLine();
    ImGui::RadioButton("Output", false);
    ImGui::End();
    // ImGui::ShowDemoWindow();
}