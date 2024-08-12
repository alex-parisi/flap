/// MidiIn.h

#pragma once

#include "../Object.h"
#include "../../widgets/Connector.h"

#include <memory>

namespace flap {
    class MidiIn : public Object {
    public:
        inline MidiIn(int port) : Object(), _port(port) {}
        void initialize() override;
        void render() override;
    private:
        int _port;
        Connector _output;
    };
}