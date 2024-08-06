/// MainApplicationSettings.h

#pragma once

#include <vector>
#include <string>

namespace flap {
    struct MainApplicationSettings {
        const std::vector<int> supportedSampleRates = {8000, 16000, 32000, 44100, 48000};
        const std::vector<int> supportedBlockSizes = {64, 128, 256, 512, 1024, 2048};
        const std::vector<std::string> supportedFormats = {"f32", "s16", "s24", "s32"};
        float sampleRate = 48000.0f;
        int blockSize = 512;
        std::string format = "f32";
    };
}