/// Chorus.h

#pragma once

#include "../Object.h"
#include "../../widgets/Connector.h"

namespace flap {
    class Chorus : public Object {
    public:
        inline Chorus(std::optional<std::string> name = std::nullopt) : Object(), _modulationDepth(2.0f), _modulationRate(0.4f), _wetMix(0.5f), _name(name) {}
        void initialize() override;
        void render() override;
    private:
        float _modulationDepth;
        float _modulationRate;
        float _wetMix;
        Connector _input;
        Connector _output;
        std::optional<std::string> _name;
    };
}