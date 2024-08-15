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
    _audioObjects.push_back(_plotObj);
    for (int i = 0; i < _numVoices; i++) {
        _generatorObjs[i] = dibiff::generator::VariableGenerator::create(flap::MainApplicationSettingsManager::getInstance().settings.blockSize, flap::MainApplicationSettingsManager::getInstance().settings.sampleRate, _state);
        _envelopeObjs[i] = dibiff::dynamic::Envelope::create(_attack, _decay, _sustain, _release, flap::MainApplicationSettingsManager::getInstance().settings.sampleRate);
    }
    /// Push the objects into the objects vector
    _audioObjects.push_back(_voiceSelectorObj);
    for (int i = 0; i < _numVoices; i++) {
        _audioObjects.push_back(_generatorObjs[i]);
        _audioObjects.push_back(_envelopeObjs[i]);
    }
    _audioObjects.push_back(_mixerObj);
    _audioObjects.push_back(_gainObj);
    _audioObjects.push_back(_plotObj);
    /// Connect the objects
    for (int i = 0; i < _numVoices; i++) {
        dibiff::graph::AudioGraph::connect(_voiceSelectorObj->getOutput(i), _generatorObjs[i]->getInput());
        dibiff::graph::AudioGraph::connect(_voiceSelectorObj->getOutput(i), _envelopeObjs[i]->getInput(1));
        dibiff::graph::AudioGraph::connect(_generatorObjs[i]->getOutput(), _envelopeObjs[i]->getInput());
        dibiff::graph::AudioGraph::connect(_envelopeObjs[i]->getOutput(), _mixerObj->getInput(i));
    }
    dibiff::graph::AudioGraph::connect(_mixerObj->getOutput(), _gainObj->getInput());
    dibiff::graph::AudioGraph::connect(_gainObj->getOutput(), _plotObj->getInput());
    /// Setup connectors
    _input = Connector(_voiceSelectorObj->getInput(), _voiceSelectorObj);
    _output = Connector(_gainObj->getOutput(), _gainObj, true);
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
    int samplesAvailable = _plotObj->ringBuffers[0]->available();
    if (samplesAvailable > 0) {
        std::vector<float> samples(samplesAvailable);
        _plotObj->ringBuffers[0]->read(samples.data(), samplesAvailable);
        ImGui::PlotLines("##", samples.data(), samplesAvailable, 0, nullptr, -1.0f, 1.0f, ImVec2(-1, -1));
    }
    ImGui::End();
    _isOpen.check();
}