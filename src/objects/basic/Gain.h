/// Gain.h

#pragma once

#include "../Object.h"

#include <memory>

namespace flap {
    class Gain : public Object {
    public:
        Gain() : Object() {}
        void initialize() override;
        void render() override;
    private:
        std::shared_ptr<float> _gain;
    };
}