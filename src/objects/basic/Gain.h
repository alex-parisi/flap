/// Gain.h

#pragma once

#include "../Object.h"

namespace flap {
    class Gain : public Object {
    public:
        Gain() : Object(), _gain(0.0f) {}
        void initialize() override;
        void render() override;
    private:
        float _gain;
    };
}