/// Envelope.h

#pragma once

#include "../Object.h"
#include "../../widgets/Connector.h"

namespace flap {
    class Envelope : public Object {
    public:
        Envelope(float& sampleRate) : Object(), _sampleRate(sampleRate), _attackTime(0.1f), _decayTime(0.1f), _sustainLevel(0.5f), _releaseTime(0.5f) {}
        void initialize() override;
        void render() override;
    private:
        float _attackTime;
        float _decayTime;
        float _sustainLevel;
        float _releaseTime;
        float& _sampleRate;
        Connector _input;
        Connector _midiInput;
        Connector _output;
    };
}