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
    _input = Connector(gainObject->getInput(), gainObject);
    _output = Connector(gainObject->getOutput(), gainObject, true);
}

void flap::Gain::render() {
    ImGui::Begin("Gain");
    ImGui::SeparatorText("Connections");
    _input.render("In");
    _output.render("Out");
    ImGui::SeparatorText("Gain Parameters");
    ImGuiKnobs::Knob("Gain", &_gain, -80.0f, 30.0f, 0.1f, "%.1fdB", ImGuiKnobVariant_Wiper);
    ImGui::End();
}