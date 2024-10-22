/// Phaser.h

#pragma once

#include "../Object.h"
#include "../../widgets/Connector.h"

namespace flap {
    class Phaser : public Object {
    public:
        inline Phaser(std::optional<std::string> name = std::nullopt) : Object(), _modulationDepth(2.0f), _modulationRate(0.4f), _baseCutoff(1000.0f), _wetMix(0.5f), _numStages(4.0f), _name(name) {}
        void initialize() override;
        void render() override;
    private:
        float _modulationDepth;
        float _modulationRate;
        float _baseCutoff;
        float _wetMix;
        float _numStages;
        Connector _input;
        Connector _output;
        std::optional<std::string> _name;
    };
}