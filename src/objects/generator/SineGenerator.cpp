/// SineGenerator.cpp

#include "SineGenerator.h"

#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "../../ConnectionService.h"

void flap::SineGenerator::initialize() {
    auto sineObject = dibiff::generator::SineGenerator::create(_settings->blockSize, _settings->sampleRate);
    _audioObjects.push_back(sineObject);
    _input = Connector(sineObject->getInput(), sineObject);
    _output = Connector(sineObject->getOutput(), sineObject, true);
}

void flap::SineGenerator::render() {
    ImGui::Begin("Sine Generator");
    ImGui::SeparatorText("Connections");
    _input.render("MIDI In");
    _output.render("Out");
    ImGui::End();
}