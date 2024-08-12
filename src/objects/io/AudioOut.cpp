/// AudioOut.cpp

#include "AudioOut.h"

#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "../../ConnectionService.h"
#include "../../managers/AudioManager.h"
#include "../../managers/GraphManager.h"
#include "../../MainApplicationSettings.h"

void flap::AudioOut::initialize() {
    auto graphSink = dibiff::sink::GraphSink::create(_channels, flap::MainApplicationSettingsManager::getInstance().settings.sampleRate, flap::MainApplicationSettingsManager::getInstance().settings.blockSize);
    _audioObjects.push_back(graphSink);
    _input = Connector(graphSink->getInput(), graphSink);
    _inputL = Connector(graphSink->getInput(0), graphSink);
    _inputR = Connector(graphSink->getInput(1), graphSink);
}

void flap::AudioOut::render() {
    std::string title;
    if (_name.has_value()) {
        title = *_name;
    } else {
        title = "AudioOut";
    }
    ImGui::Begin(title.c_str(), _isOpen);
    ImGui::SeparatorText("Connections");
    if (_channels == 1) {
        _input.render("In");
    } else {
        _inputL.render("In-L");
        ImGui::SameLine();
        _inputR.render("In-R");
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

void flap::AudioOut::preDisconnect() {
    /// Remove graph mutexs and condition variables
    auto sink = std::dynamic_pointer_cast<dibiff::sink::GraphSink>(_audioObjects[0]);
    flap::GraphManager::getInstance().removeGraphMutex(&sink->cv_mtx);
    flap::GraphManager::getInstance().removeGraphSignal(&sink->cv);
}

void flap::AudioOut::postDisconnect() {
    /// Close the playback device
    flap::AudioManager::getInstance().closePlaybackDevice(_device);
}