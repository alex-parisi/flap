/// MidiIn.h

#pragma once

#include "../Object.h"

#include <memory>

namespace flap {
    class MidiIn : public Object {
    public:
        MidiIn(int port, int blockSize) : Object(), _port(port), _blockSize(blockSize) {}
        void initialize() override;
        void render() override;
    private:
        int _port;
        int _blockSize;
        bool _outputSelected = false;
    };
}