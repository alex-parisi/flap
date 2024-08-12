/// Probe.cpp

#include "Probe.h"

#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "../../ConnectionService.h"
#include "../../managers/AudioManager.h"
#include "../../managers/GraphManager.h"
#include "../../MainApplicationSettings.h"

void flap::Probe::initialize() {
    auto graphSink = dibiff::sink::GraphSink::create(1, flap::MainApplicationSettingsManager::getInstance().settings.sampleRate, flap::MainApplicationSettingsManager::getInstance().settings.blockSize);
    _audioObjects.push_back(graphSink);
    _input = Connector(graphSink->getInput(), graphSink);
}

void flap::Probe::render() {
    std::string title;
    if (_name.has_value()) {
        title = *_name;
    } else {
        title = "Probe";
    }
    ImGui::Begin(title.c_str(), _isOpen);
    ImGui::SeparatorText("Connections");
    _input.render("In");
    ImGui::SeparatorText("Probe Output");
    /// Plot the output of the probe
    auto sink = std::dynamic_pointer_cast<dibiff::sink::GraphSink>(_audioObjects[0]);
    int samplesAvailable = sink->ringBuffers[0]->available();
    if (samplesAvailable > 0) {
        std::vector<float> samples(samplesAvailable);
        sink->ringBuffers[0]->read(samples.data(), samplesAvailable);
        ImGui::PlotLines("##", samples.data(), samplesAvailable, 0, nullptr, -1.0f, 1.0f, ImVec2(-1, -1));
    }
    ImGui::End();
    _isOpen.check();
}