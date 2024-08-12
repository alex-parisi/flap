/// Gain.h

#pragma once

#include "../Object.h"
#include "../../widgets/Connector.h"

namespace flap {
    class Gain : public Object {
    public:
        inline Gain(std::optional<std::string> name = std::nullopt) : Object(), _gain(0.0f), _name(name) {}
        void initialize() override;
        void render() override;
    private:
        float _gain;
        Connector _input;
        Connector _output;
        std::optional<std::string> _name;
    };
}