/// AudioOut.cpp

#include "AudioOut.h"

#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "../../ConnectionService.h"

void flap::AudioOut::initialize() {
    auto graphSink = dibiff::sink::GraphSink::create(_channels, _sampleRate, _blockSize);
    _audioObjects.push_back(graphSink);
    _input = Connector(graphSink->getInput(), graphSink);
    _inputL = Connector(graphSink->getInput(0), graphSink);
    _inputR = Connector(graphSink->getInput(1), graphSink);
}

void flap::AudioOut::render() {
    ImGui::Begin("AudioOut");
    ImGui::SeparatorText("Connections");
    if (_channels == 1) {
        _input.render("In");
    } else {
        _inputL.render("In L");
        _inputR.render("In R");
    }
    ImGui::End();
}