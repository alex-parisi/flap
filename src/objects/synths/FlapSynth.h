/// FlapSynth.h

#pragma once

#include "../Object.h"
#include "../../MainApplicationSettings.h"
#include "../../widgets/Connector.h"

#include <memory>

namespace flap {
    class FlapSynth : public Object {
        public:
            inline FlapSynth(std::optional<std::string> name = std::nullopt) : Object(), _name(name), _numVoices(16), _gain(0.0f), _attack(0.1f), _decay(0.1f), _sustain(0.5f), _release(0.5f), _state(0.0f), _dutyCycle(0.5f) {}
            void initialize() override;
            void render() override;
        private:
            Connector _input;
            Connector _output;
            std::optional<std::string> _name;
            std::unique_ptr<dibiff::midi::VoiceSelector> _voiceSelectorObj;
            std::unique_ptr<dibiff::level::Mixer> _mixerObj;
            std::unique_ptr<dibiff::level::Gain> _gainObj;
            std::vector<std::unique_ptr<dibiff::generator::VariableGenerator>> _generatorObjs;
            std::vector<std::unique_ptr<dibiff::dynamic::Envelope>> _envelopeObjs;
            std::unique_ptr<dibiff::sink::GraphSink> _plotObj;
            int _numVoices;
            float _gain;
            float _attack;
            float _decay;
            float _sustain;
            float _release;
            float _state;
            float _dutyCycle;
    };
}