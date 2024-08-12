/// Gain.h

#pragma once

#include "../Object.h"
#include "../../widgets/Connector.h"

namespace flap {
    class Gain : public Object {
    public:
        inline Gain() : Object(), _gain(0.0f) {}
        void initialize() override;
        void render() override;
    private:
        float _gain;
        Connector _input;
        Connector _output;
    };
}