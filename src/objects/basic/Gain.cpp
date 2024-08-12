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
        std::string title;
    if (_name.has_value()) {
        title = *_name;
    } else {
        title = "Gain";
    }
    ImGui::Begin(title.c_str(), _isOpen);
    ImGui::SeparatorText("Connections");
    _input.render("In");
    ImGui::SameLine();
    _output.render("Out");
    ImGui::SeparatorText("Gain Parameters");
    ImGuiKnobs::Knob("Gain", &_gain, -80.0f, 30.0f, 0.1f, "%.1fdB", ImGuiKnobVariant_Wiper);
    ImGui::End();
    _isOpen.check();
}