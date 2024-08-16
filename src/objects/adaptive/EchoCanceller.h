/// EchoCanceller.h

#pragma once

#include "../Object.h"
#include "../../widgets/Connector.h"

namespace flap {
    class EchoCanceller : public Object {
    public:
        inline EchoCanceller(std::optional<std::string> name = std::nullopt) : Object(), _filterLength(512), _stepSize(0.01), _name(name) {}
        void initialize() override;
        void render() override;
    private:
        int _filterLength;
        float _stepSize;
        Connector _input;
        Connector _reference;
        Connector _output;
        std::optional<std::string> _name;
    };
}