/// AudioOut.h

#pragma once

#include "../Object.h"
#include "miniaudio.h"

#include <memory>

namespace flap {
    class AudioOut : public Object {
    public:
        AudioOut(int id, int channels, int sampleRate, int blockSize) : Object(), _id(id), _channels(channels), _sampleRate(sampleRate), _blockSize(blockSize) {}
        void initialize() override;
        void render() override;
    private:
        int _id;
        int _channels;
        int _sampleRate;
        int _blockSize;
        bool _inputSelected = false;
        bool _inputLSelected = false;
        bool _inputRSelected = false;
    };
}