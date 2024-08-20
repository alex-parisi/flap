/// SineGenerator.cpp

#include "SineGenerator.h"

#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "../../ConnectionService.h"
#include "../../MainApplicationSettings.h"

void flap::SineGenerator::initialize() {
    auto sineObject = dibiff::generator::SineGenerator::create(flap::MainApplicationSettingsManager::getInstance().settings.blockSize, flap::MainApplicationSettingsManager::getInstance().settings.sampleRate);
    _audioObjects.push_back(std::move(sineObject));
    _input = Connector(_audioObjects[0].get()->getInput(), _audioObjects[0].get());
    _output = Connector(_audioObjects[0].get()->getOutput(), _audioObjects[0].get(), true);
}

void flap::SineGenerator::render() {
        std::string title;
    if (_name.has_value()) {
        title = *_name;
    } else {
        title = "SineGenerator";
    }
    ImGui::Begin(title.c_str(), _isOpen);
    ImGui::SeparatorText("Connections");
    _input.render("MIDI-In");
    ImGui::SameLine();
    _output.render("Out");
    ImGui::End();
    _isOpen.check();
}