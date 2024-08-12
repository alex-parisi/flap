/// Flanger.h

#pragma once

#include "../Object.h"
#include "../../widgets/Connector.h"

namespace flap {
    class Flanger : public Object {
    public:
        Flanger() : Object(), _modulationDepth(2.0f), _modulationRate(0.4f), _feedback(0.7f), _wetMix(0.5f) {}
        void initialize() override;
        void render() override;
    private:
        float _modulationDepth;
        float _modulationRate;
        float _feedback;
        float _wetMix;
        Connector _input;
        Connector _output;
    };
}