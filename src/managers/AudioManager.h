/// AudioManager.h

#pragma once

#include "Manager.h"

#include <condition_variable>
#include <mutex>
#include <vector>
#include <chrono>
#include <memory>

#include "miniaudio.h"
#include "dibiff/dibiff"

#include "../MainApplicationSettings.h"

namespace flap {
    class AudioManager : public Manager {
        public: 
            AudioManager(std::shared_ptr<MainApplicationSettings> settings) : Manager(settings) {}
            bool initialize() override;
            void cleanup() override;
            std::shared_ptr<dibiff::sink::GraphSink> getGraphSink() { return _graphSink; }
            bool selectPlaybackDevice(int index);
            bool selectCaptureDevice(int index);
            std::vector<ma_device_info> getPlaybackDevices();
            std::vector<ma_device_info> getCaptureDevices();
        private:
            void _threadFunction() override;
            std::shared_ptr<dibiff::sink::GraphSink> _graphSink;
            ma_device_config _playbackConfig;
            ma_device _playbackDevice;
            static void _playbackDataCallback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount);
            ma_device_config _captureConfig;
            ma_device _captureDevice;
            static void _captureDataCallback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount);
            ma_context _playbackContext;
            ma_context _captureContext;
            std::vector<ma_device_info> _playbackDevices;
            std::vector<ma_device_info> _captureDevices;
            std::chrono::duration<int, std::milli> _updateInterval = std::chrono::milliseconds(1000);
    };
}