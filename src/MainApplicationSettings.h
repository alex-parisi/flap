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
    class MainApplicationSettingsManager {
        public:
            /// Singleton pattern
            static MainApplicationSettingsManager& getInstance() {
                static MainApplicationSettingsManager instance; // Guaranteed to be destroyed and instantiated on first use
                return instance;
            }
            /// Delete copy constructor and assignment operator to prevent copying
            MainApplicationSettingsManager(const MainApplicationSettingsManager&) = delete;
            MainApplicationSettingsManager& operator=(const MainApplicationSettingsManager&) = delete;
            MainApplicationSettings settings;
        private:
            /// Singleton pattern
            MainApplicationSettingsManager() {}
            ~MainApplicationSettingsManager() {}
    };
}