/// KeyboardSimulator.cpp

#include "KeyboardSimulator.h"

#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "../../ConnectionService.h"
#include "../../MainApplicationSettings.h"
#include "../../KeyboardService.h"

void flap::KeyboardSimulator::initialize() {
    auto midiObject = dibiff::midi::MidiInput::create(flap::MainApplicationSettingsManager::getInstance().settings.blockSize);
    _audioObjects.push_back(std::move(midiObject));
    _output = Connector(_audioObjects[0].get()->getOutput(), _audioObjects[0].get(), true);
}

void flap::KeyboardSimulator::render() {
    update();
    std::string title;
    if (_name.has_value()) {
        title = *_name;
    } else {
        title = "KeyboardSimulator";
    }
    ImGui::Begin(title.c_str(), _isOpen);
    ImGui::SeparatorText("Connections");
    _output.render("MIDI-Out");
    ImGui::End();
    _isOpen.check();
}

void flap::KeyboardSimulator::update() {
    for (auto& note : _midiNotes) {
        int noteVal = flap::KeyboardService::getInstance().isKeyDown((ImGuiKey)note.first);
        if (noteVal == 2) {
            unsigned char message[3];
            message[0] = 0x90; // Note on
            message[1] = note.second; // Note number
            message[2] = 127; // Velocity
            std::vector<unsigned char> m = { message[0], message[1], message[2] };
            std::vector<std::vector<unsigned char>> messages = { m };
            auto mi = static_cast<dibiff::midi::MidiInput*>(_audioObjects[0].get());
            mi->addMidiMessage(messages);
        } else if (noteVal == -1) {
            unsigned char message[3];
            message[0] = 0x80; // Note off
            message[1] = note.second; // Note number
            message[2] = 0; // Velocity
            std::vector<unsigned char> m = { message[0], message[1], message[2] };
            std::vector<std::vector<unsigned char>> messages = { m };
            auto mi = static_cast<dibiff::midi::MidiInput*>(_audioObjects[0].get());
            mi->addMidiMessage(messages);
        }
    }
}