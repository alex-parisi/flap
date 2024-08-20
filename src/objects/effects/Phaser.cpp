/// Phaser.cpp

#include "Phaser.h"

#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui-knobs.h"

#include "../../ConnectionService.h"
#include "../../MainApplicationSettings.h"

void flap::Phaser::initialize() {
    int numStages = static_cast<int>(_numStages);
    auto phaserObject = dibiff::effect::Phaser::create(_modulationDepth, _modulationRate, flap::MainApplicationSettingsManager::getInstance().settings.sampleRate, _baseCutoff, _wetMix, numStages);
    _audioObjects.push_back(std::move(phaserObject));
    _input = Connector(_audioObjects[0].get()->getInput(), _audioObjects[0].get());
    _output = Connector(_audioObjects[0].get()->getOutput(), _audioObjects[0].get(), true);
}

void flap::Phaser::render() {
        std::string title;
    if (_name.has_value()) {
        title = *_name;
    } else {
        title = "Phaser";
    }
    ImGui::Begin(title.c_str(), _isOpen);
    ImGui::SeparatorText("Connections");
    _input.render("In");
    ImGui::SameLine();
    _output.render("Out");
    ImGui::SeparatorText("Phaser Parameters");
    ImGuiKnobs::Knob("Mod. Depth", &_modulationDepth, 0.0f, 5.0f, 0.05f, "%.3fms", ImGuiKnobVariant_Wiper);
    ImGui::SameLine();
    ImGuiKnobs::Knob("Mod. Rate", &_modulationRate, 0.0f, 10.0f, 0.1f, "%.1fHz", ImGuiKnobVariant_Wiper);
    ImGui::SameLine();
    /// TODO: Log knob
    ImGuiKnobs::Knob("Base Cutoff", &_baseCutoff, 20.0f, 20000.0f, 1.0f, "%.1f", ImGuiKnobVariant_Wiper);
    ImGui::SameLine();
    ImGuiKnobs::Knob("Mix", &_wetMix, 0.0f, 1.0f, 0.01f, "%.01f", ImGuiKnobVariant_Wiper);
    ImGui::SameLine();
    ImGuiKnobs::Knob("# Stages", &_numStages, 1.0, 10.0f, 1.0f, "%d", ImGuiKnobVariant_Stepped);
    ImGui::End();
    _isOpen.check();
}