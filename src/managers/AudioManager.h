/// AudioManager.h

#pragma once

#include "Manager.h"

#include <condition_variable>
#include <mutex>
#include <vector>
#include <chrono>
#include <memory>
#include <map>

#include "miniaudio.h"
#include "dibiff/dibiff"
#include "../objects/io/AudioOut.h"
#include "../MainApplicationSettings.h"

namespace flap {
    struct AudioCallbackData;
    class AudioManager : public Manager {
        public: 
            AudioManager(std::shared_ptr<MainApplicationSettings> settings) : Manager(settings) {}
            bool initialize() override;
            void cleanup() override;
            std::vector<ma_device_info> getPlaybackDevices();
            std::vector<ma_device_info> getCaptureDevices();
            std::optional<std::shared_ptr<AudioOut>> openPlaybackDevice(ma_device_info device, ma_format format, int channels, int sampleRate);
        private:
            ma_context _playbackContext;
            ma_context _captureContext;
            std::vector<ma_device_info> _playbackDeviceInfos;
            std::vector<ma_device_info> _captureDeviceInfos;
            ma_device_config _playbackConfig;
            ma_device _playbackDevice;
            std::shared_ptr<AudioOut> _audioOut;
            std::chrono::duration<int, std::milli> _updateInterval = std::chrono::milliseconds(1000);
            void _threadFunction() override;
            static void _playbackDataCallback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount);
    };
    struct AudioCallbackData {
        flap::AudioManager* manager;
        int id;
    };
}