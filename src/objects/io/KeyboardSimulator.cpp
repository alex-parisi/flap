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
    _audioObjects.push_back(midiObject);
    _output = Connector(midiObject->getOutput(), midiObject, true);
}

void flap::KeyboardSimulator::render() {
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
            auto a = _audioObjects[0];
            auto b = std::dynamic_pointer_cast<dibiff::midi::MidiInput>(a);
            b->addMidiMessage(m);
        } else if (noteVal == -1) {
            unsigned char message[3];
            message[0] = 0x80; // Note off
            message[1] = note.second; // Note number
            message[2] = 0; // Velocity
            std::vector<unsigned char> m = { message[0], message[1], message[2] };
            auto a = _audioObjects[0];
            auto b = std::dynamic_pointer_cast<dibiff::midi::MidiInput>(a);
            b->addMidiMessage(m);
        }
    }
}