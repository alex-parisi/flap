/// MidiIn.cpp

#include "MidiIn.h"

#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "../../ConnectionService.h"
#include "../../MainApplicationSettings.h"

void flap::MidiIn::initialize() {
    auto midiObject = dibiff::midi::MidiInput::create(flap::MainApplicationSettingsManager::getInstance().settings.blockSize);
    _audioObjects.push_back(midiObject);
    _output = Connector(midiObject->getOutput(), midiObject, true);
}

void flap::MidiIn::render() {
        std::string title;
    if (_name.has_value()) {
        title = *_name;
    } else {
        title = "MidiIn";
    }
    ImGui::Begin(title.c_str(), _isOpen);
    ImGui::SeparatorText("Connections");
    _output.render("MIDI-Out");
    ImGui::End();
    _isOpen.check();
}