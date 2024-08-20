/// FlapSynth.cpp

#include "FlapSynth.h"

#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui-knobs.h"

#include "../../ConnectionService.h"
#include "../../MainApplicationSettings.h"

void flap::FlapSynth::initialize() {
    /// Create the objects
    _generatorObjs.resize(_numVoices);
    _envelopeObjs.resize(_numVoices);
    _voiceSelectorObj = dibiff::midi::VoiceSelector::create(flap::MainApplicationSettingsManager::getInstance().settings.blockSize, _numVoices);
    _mixerObj = dibiff::level::Mixer::create(_numVoices);
    _gainObj= dibiff::level::Gain::create(_gain);
    _plotObj = dibiff::sink::GraphSink::create(1, flap::MainApplicationSettingsManager::getInstance().settings.sampleRate, flap::MainApplicationSettingsManager::getInstance().settings.blockSize);
    for (int i = 0; i < _numVoices; i++) {
        _generatorObjs[i] = dibiff::generator::VariableGenerator::create(flap::MainApplicationSettingsManager::getInstance().settings.blockSize, flap::MainApplicationSettingsManager::getInstance().settings.sampleRate, _state);
        _envelopeObjs[i] = dibiff::dynamic::Envelope::create(_attack, _decay, _sustain, _release, flap::MainApplicationSettingsManager::getInstance().settings.sampleRate);
    }
    /// Push the objects into the objects vector
    _audioObjects.push_back(std::move(_voiceSelectorObj)); // 0
    _audioObjects.push_back(std::move(_gainObj)); // 1
    _audioObjects.push_back(std::move(_mixerObj)); // 2
    _audioObjects.push_back(std::move(_plotObj)); // 3
    for (int i = 0; i < _numVoices; i++) {
        _audioObjects.push_back(std::move(_generatorObjs[i]));
        _audioObjects.push_back(std::move(_envelopeObjs[i]));
    }
    /// Connect the objects
    int j = 0;
    for (int i = 0; i < _numVoices; i++) {
        dibiff::graph::AudioGraph::connect(_audioObjects[0].get()->getOutput(i), _audioObjects[4 + j].get()->getInput());
        dibiff::graph::AudioGraph::connect(_audioObjects[0].get()->getOutput(i), _audioObjects[5 + j].get()->getInput(1));
        dibiff::graph::AudioGraph::connect(_audioObjects[4 + j].get()->getOutput(), _audioObjects[5 + j].get()->getInput());
        dibiff::graph::AudioGraph::connect(_audioObjects[5 + j].get()->getOutput(), _audioObjects[2].get()->getInput(i));
        ++++j;
    }
    dibiff::graph::AudioGraph::connect(_audioObjects[2].get()->getOutput(), _audioObjects[1].get()->getInput());
    dibiff::graph::AudioGraph::connect(_audioObjects[1].get()->getOutput(), _audioObjects[3].get()->getInput());
    /// Setup connectors
    _input = Connector(_audioObjects[0].get()->getInput(), _audioObjects[0].get());
    _output = Connector(_audioObjects[1].get()->getOutput(), _audioObjects[1].get(), true);
}

void flap::FlapSynth::render() {
        std::string title;
    if (_name.has_value()) {
        title = *_name;
    } else {
        title = "FlapSynth";
    }
    ImGui::Begin(title.c_str(), _isOpen);
    ImGui::SeparatorText("Connections");
    _input.render("MIDI-In");
    ImGui::SameLine();
    _output.render("Out");
    ImGui::SeparatorText("Synth Settings");
    ImGuiKnobs::Knob("Gain", &_gain, -80.0f, 30.0f, 0.1f, "%.1fdB", ImGuiKnobVariant_Wiper);
    ImGui::SameLine();
    ImGuiKnobs::Knob("Attack", &_attack, 0.005f, 2.0f, 0.005f, "%1.3fs", ImGuiKnobVariant_Wiper);
    ImGui::SameLine();
    ImGuiKnobs::Knob("Decay", &_decay, 0.005f, 2.0f, 0.005f, "%1.3fs", ImGuiKnobVariant_Wiper);
    ImGui::SameLine();
    ImGuiKnobs::Knob("Sustain", &_sustain, 0.005f, 1.0f, 0.005f, "%1.3f", ImGuiKnobVariant_Wiper);
    ImGui::SameLine();
    ImGuiKnobs::Knob("Release", &_release, 0.01f, 5.0f, 0.01f, "%1.2fs", ImGuiKnobVariant_Wiper);
    ImGui::SameLine();
    ImGuiKnobs::Knob("Shape", &_state, 0.0, 2.0f, 0.01f, "%1.0f", ImGuiKnobVariant_Stepped, 0.0f, 0, 3);
    ImGui::SeparatorText("Synth Output");
    auto& plotObj = static_cast<dibiff::sink::GraphSink&>(*_audioObjects[3]);
    int samplesAvailable = plotObj.ringBuffers[0]->available();
    if (samplesAvailable > 0) {
        std::vector<float> samples = plotObj.ringBuffers[0]->read(samplesAvailable);
        ImGui::PlotLines("##", samples.data(), samplesAvailable, 0, nullptr, -1.0f, 1.0f, ImVec2(-1, -1));
    }
    ImGui::End();
    _isOpen.check();
}