/// AudioManager.cpp

#define MINIAUDIO_IMPLEMENTATION

#include "AudioManager.h"

#include <iostream>

bool AudioManager::initialize() {
    ma_result result;
    // Initialize playback context
    result = ma_context_init(NULL, 0, NULL, &_playbackContext);
    if (result != MA_SUCCESS) {
        std::cerr << "Failed to initialize playback context." << std::endl;
        return false;
    }
    // Initialize capture context
    result = ma_context_init(NULL, 0, NULL, &_captureContext);
    if (result != MA_SUCCESS) {
        std::cerr << "Failed to initialize capture context." << std::endl;
        return false;
    }
    return true;
}

void AudioManager::cleanup() {
    // ma_device_uninit(&_playbackDevice);
    // ma_device_uninit(&_captureDevice);
    ma_context_uninit(&_playbackContext);
    ma_context_uninit(&_captureContext);
}

// bool AudioManager::selectPlaybackDevice(int index) {
//     ma_device_info* pPlaybackDevices;
//     ma_uint32 playbackDeviceCount;
//     ma_result result;
//     result = ma_context_get_devices(&_playbackContext, &pPlaybackDevices, &playbackDeviceCount, NULL, NULL);
//     if (result != MA_SUCCESS) {
//         std::cerr << "Failed to enumerate playback devices." << std::endl;
//         return false;
//     }
//     if (index < 0 || index >= playbackDeviceCount) {
//         std::cerr << "Invalid playback device index." << std::endl;
//         return false;
//     }
//     /// Determine the supported formats and channels for the device

//     _playbackConfig = ma_device_config_init(ma_device_type_playback);
//     _playbackConfig.playback.format = ma_format_f32;
//     _playbackConfig.playback.channels = 2;
//     _playbackConfig.sampleRate = _sampleRate;
//     _playbackConfig.dataCallback = _playbackDataCallback;
//     _playbackConfig.pUserData = this;
//     _playbackConfig.playback.pDeviceID = &pPlaybackDevices[index].id;
//     result = ma_device_init(&_playbackContext, &_playbackConfig, &_playbackDevice);
//     if (result != MA_SUCCESS) {
//         std::cerr << "Failed to initialize playback device." << std::endl;
//         return false;
//     }
//     result = ma_device_start(&_playbackDevice);
//     if (result != MA_SUCCESS) {
//         std::cerr << "Failed to start playback device." << std::endl;
//         ma_device_uninit(&_playbackDevice);
//         return false;
//     }
//     return true;
// }

// bool AudioManager::selectCaptureDevice(int index) {
//     ma_device_info* pCaptureDevices;
//     ma_uint32 captureDeviceCount;
//     ma_result result;
//     result = ma_context_get_devices(&_captureContext, NULL, NULL, &pCaptureDevices, &captureDeviceCount);
//     if (result != MA_SUCCESS) {
//         std::cerr << "Failed to enumerate capture devices." << std::endl;
//         return false;
//     }
//     if (index < 0 || index >= captureDeviceCount) {
//         std::cerr << "Invalid capture device index." << std::endl;
//         return false;
//     }
//     _captureConfig = ma_device_config_init(ma_device_type_capture);
//     _captureConfig.capture.format = ma_format_f32;
//     _captureConfig.capture.channels = 2;
//     _captureConfig.sampleRate = _sampleRate;
//     _captureConfig.dataCallback = _captureDataCallback;
//     _captureConfig.pUserData = this;
//     _captureConfig.capture.pDeviceID = &pCaptureDevices[index].id;
//     result = ma_device_init(&_captureContext, &_captureConfig, &_captureDevice);
//     if (result != MA_SUCCESS) {
//         std::cerr << "Failed to initialize capture device." << std::endl;
//         return false;
//     }
//     result = ma_device_start(&_captureDevice);
//     if (result != MA_SUCCESS) {
//         std::cerr << "Failed to start capture device." << std::endl;
//         ma_device_uninit(&_captureDevice);
//         return false;
//     }
//     return true;
// }

void AudioManager::_threadFunction() {
    while (isRunning()) {
        /// Mutex Locked
        {
            std::lock_guard<std::mutex> lock(_mutex);
            /// Check for playback and capture devices, and update the lists
            ma_device_info* pPlaybackDevices;
            ma_uint32 playbackDeviceCount;
            ma_result result;
            result = ma_context_get_devices(&_playbackContext, &pPlaybackDevices, &playbackDeviceCount, NULL, NULL);
            if (result != MA_SUCCESS) {
                std::cerr << "Failed to enumerate playback devices." << std::endl;
                return;
            }
            _playbackDevices.clear();
            for (ma_uint32 i = 0; i < playbackDeviceCount; i++) {
                _playbackDevices.push_back(pPlaybackDevices[i]);
            }
            ma_device_info* pCaptureDevices;
            ma_uint32 captureDeviceCount;
            result = ma_context_get_devices(&_captureContext, NULL, NULL, &pCaptureDevices, &captureDeviceCount);
            if (result != MA_SUCCESS) {
                std::cerr << "Failed to enumerate capture devices." << std::endl;
                return;
            }
            _captureDevices.clear();
            for (ma_uint32 i = 0; i < captureDeviceCount; i++) {
                _captureDevices.push_back(pCaptureDevices[i]);
            }
        }
        /// Mutex Unlocked
        /// Sleep for the update interval
        std::this_thread::sleep_for(_updateInterval);
    }
}

// void AudioManager::_playbackDataCallback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount) {
// }

// void AudioManager::_captureDataCallback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount) {
// }