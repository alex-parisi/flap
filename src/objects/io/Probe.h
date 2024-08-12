/// Probe.h

#pragma once

#include "../Object.h"
#include "../../widgets/Connector.h"

#include <memory>
#include <optional>
#include <string>

namespace flap {
    class Probe : public Object {
    public:
        inline Probe(std::optional<std::string> name = std::nullopt) : Object(), _name(name) {}
        void initialize() override;
        void render() override;
    private:
        Connector _input;
        std::optional<std::string> _name;
    };
}