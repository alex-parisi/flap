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
    _audioObjects.push_back(sineObject);
    _input = Connector(sineObject->getInput(), sineObject);
    _output = Connector(sineObject->getOutput(), sineObject, true);
}

void flap::SineGenerator::render() {
    ImGui::Begin("Sine Generator", _isOpen);
    ImGui::SeparatorText("Connections");
    _input.render("MIDI-In");
    ImGui::SameLine();
    _output.render("Out");
    ImGui::End();
    _isOpen.check();
}