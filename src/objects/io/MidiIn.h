/// MidiIn.h

#pragma once

#include "../Object.h"
#include "../../widgets/Connector.h"

#include <memory>

namespace flap {
    class MidiIn : public Object {
    public:
        inline MidiIn(int port, std::optional<std::string> name = std::nullopt) : Object(), _port(port), _name(name) {}
        void initialize() override;
        void render() override;
    private:
        int _port;
        Connector _output;
        std::optional<std::string> _name;
    };
}