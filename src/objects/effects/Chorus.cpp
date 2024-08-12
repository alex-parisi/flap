/// Chorus.cpp

#include "Chorus.h"

#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui-knobs.h"

#include "../../ConnectionService.h"
#include "../../MainApplicationSettings.h"

void flap::Chorus::initialize() {
    auto chorusObject = dibiff::effect::Chorus::create(_modulationDepth, _modulationRate, flap::MainApplicationSettingsManager::getInstance().settings.sampleRate, _wetMix);
    _audioObjects.push_back(chorusObject);
    _input = Connector(chorusObject->getInput(), chorusObject);
    _output = Connector(chorusObject->getOutput(), chorusObject, true);
}

void flap::Chorus::render() {
        std::string title;
    if (_name.has_value()) {
        title = *_name;
    } else {
        title = "Chorus";
    }
    ImGui::Begin(title.c_str(), _isOpen);
    ImGui::SeparatorText("Connections");
    _input.render("In");
    ImGui::SameLine();
    _output.render("Out");
    ImGui::SeparatorText("Chorus Parameters");
    ImGuiKnobs::Knob("Mod. Depth", &_modulationDepth, 0.0f, 5.0f, 0.05f, "%.3fms", ImGuiKnobVariant_Wiper);
    ImGui::SameLine();
    ImGuiKnobs::Knob("Mod. Rate", &_modulationRate, 0.0f, 10.0f, 0.1f, "%.1fHz", ImGuiKnobVariant_Wiper);
    ImGui::SameLine();
    ImGuiKnobs::Knob("Mix", &_wetMix, 0.0f, 1.0f, 0.01f, "%.01f", ImGuiKnobVariant_Wiper);
    ImGui::End();
    _isOpen.check();
}