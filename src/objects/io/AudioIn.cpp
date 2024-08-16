/// AudioIn.cpp

#include "AudioIn.h"

#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "../../ConnectionService.h"
#include "../../managers/AudioManager.h"
#include "../../managers/GraphManager.h"
#include "../../MainApplicationSettings.h"

void flap::AudioIn::initialize() {
    auto graphSource = dibiff::source::GraphSource::create(_channels, flap::MainApplicationSettingsManager::getInstance().settings.sampleRate, flap::MainApplicationSettingsManager::getInstance().settings.blockSize);
    _audioObjects.push_back(graphSource);
    _output = Connector(graphSource->getOutput(), graphSource, true);
    _outputL = Connector(graphSource->getOutput(0), graphSource, true);
    _outputR = Connector(graphSource->getOutput(1), graphSource, true);
}

void flap::AudioIn::render() {
    std::string title;
    if (_name.has_value()) {
        title = *_name;
    } else {
        title = "AudioIn";
    }
    ImGui::Begin(title.c_str(), _isOpen);
    ImGui::SeparatorText("Connections");
    if (_channels == 1) {
        _output.render("Out");
    } else {
        _outputL.render("Out-L");
        ImGui::SameLine();
        _outputR.render("Out-R");
    }
    ImGui::SeparatorText("Audio Settings");
    if (ImGui::RadioButton("Mono", _channels == 1)) {
        if (_channels != 1) {
            _channels = 1;
            /// TODO: Change device and object to mono
        }
    }
    ImGui::SameLine();
    if (ImGui::RadioButton("Stereo", _channels == 2)) {
        if (_channels != 2) {
            _channels = 2;
            /// TODO: Change device and object to stereo
        }
    }
    ImGui::End();
    _isOpen.check();
}

void flap::AudioIn::preDisconnect() {
    /// Remove graph mutexs and condition variables
    auto source = std::dynamic_pointer_cast<dibiff::source::GraphSource>(_audioObjects[0]);
    flap::GraphManager::getInstance().removeInputGraphMutex(&source->cv_mtx);
    flap::GraphManager::getInstance().removeInputGraphSignal(&source->cv);
}

void flap::AudioIn::postDisconnect() {
    /// Close the playback device
    flap::AudioManager::getInstance().closeCaptureDevice(_device);
}