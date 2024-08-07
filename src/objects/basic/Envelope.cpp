/// Envelope.cpp

#include "Envelope.h"

#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui-knobs.h"

#include "../../ConnectionService.h"

void flap::Envelope::initialize() {
    auto envelope = dibiff::dynamic::Envelope::create(_attackTime, _decayTime, _sustainLevel, _releaseTime, _sampleRate);
    _audioObjects.push_back(envelope);
    _input = Connector(envelope->getInput(0), envelope);
    _midiInput = Connector(envelope->getInput(1), envelope);
    _output = Connector(envelope->getOutput(), envelope, true);
}

void flap::Envelope::render() {
    ImGui::Begin("Envelope");
    ImGui::SeparatorText("Connections");
    _midiInput.render("MIDI In");
    _input.render("In");
    _output.render("Out");
    ImGui::SeparatorText("Envelope Parameters");
    ImGuiKnobs::Knob("Attack", &_attackTime, 0.0f, 2.0f, 0.005f, "%1.3fs", ImGuiKnobVariant_Wiper);
    ImGui::SameLine();
    ImGuiKnobs::Knob("Decay", &_decayTime, 0.0f, 2.0f, 0.005f, "%1.3fs", ImGuiKnobVariant_Wiper);
    ImGui::SameLine();
    ImGuiKnobs::Knob("Sustain", &_sustainLevel, 0.0f, 1.0f, 0.005f, "%1.3f", ImGuiKnobVariant_Wiper);
    ImGui::SameLine();
    ImGuiKnobs::Knob("Release", &_releaseTime, 0.0f, 5.0f, 0.01f, "%1.2fs", ImGuiKnobVariant_Wiper);
    ImGui::End();
}