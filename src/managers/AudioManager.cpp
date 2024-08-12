/// AudioManager.cpp

#define MINIAUDIO_IMPLEMENTATION

#include "AudioManager.h"
#include <iostream>
#include <cstring>
#include "../MainApplicationSettings.h"

bool flap::AudioManager::initialize() {
    _mutex = std::make_shared<std::mutex>();
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

void flap::AudioManager::cleanup() {
    for (auto& pd : _playbackDevices) {
        ma_device_state state = ma_device_get_state(pd.second.get());
        if (state == ma_device_state_started) {
            ma_device_stop(pd.second.get());
            ma_device_uninit(pd.second.get());
        }
    }
    ma_context_uninit(&_playbackContext);
    
    ma_context_uninit(&_captureContext);
}

std::vector<ma_device_info> flap::AudioManager::getPlaybackDevices() {
    std::lock_guard<std::mutex> lock(*_mutex);
    return _playbackDeviceInfos;
}

std::vector<ma_device_info> flap::AudioManager::getCaptureDevices() {
    std::lock_guard<std::mutex> lock(*_mutex);
    return _captureDeviceInfos;
}

std::optional<std::shared_ptr<flap::AudioOut>> flap::AudioManager::openPlaybackDevice(ma_device_info device, ma_format format, int channels) {
    std::lock_guard<std::mutex> lock(*_mutex);

    ma_result result;
    auto audioCallbackData = std::make_shared<AudioCallbackData>();
    audioCallbackData->device = device;
    _audioCallbackDatas[device.name] = audioCallbackData;
    /// TODO: Determine the supported formats and channels for the device
    /// TODO: Support stereo when selected 
    std::shared_ptr<ma_device> playbackDevice = std::make_shared<ma_device>();
    std::shared_ptr<ma_device_config> playbackConfig = std::make_shared<ma_device_config>();
    *playbackConfig = ma_device_config_init(ma_device_type_playback);
    playbackConfig->playback.format = ma_format_f32;
    playbackConfig->playback.channels = channels;
    playbackConfig->sampleRate = flap::MainApplicationSettingsManager::getInstance().settings.sampleRate;
    playbackConfig->dataCallback = _playbackDataCallback;
    playbackConfig->pUserData = audioCallbackData.get();
    playbackConfig->playback.pDeviceID = &device.id;
    playbackConfig->periodSizeInFrames = flap::MainApplicationSettingsManager::getInstance().settings.blockSize;
    _playbackConfigs[device.name] = playbackConfig;
    result = ma_device_init(&_playbackContext, playbackConfig.get(), playbackDevice.get());
    if (result != MA_SUCCESS) {
        std::cerr << "Failed to initialize playback device." << std::endl;
        return std::nullopt;
    }
    std::cout << "Opened playback device: " << device.name << std::endl;
    std::shared_ptr<AudioOut> audioOut = std::make_shared<AudioOut>(device, channels, device.name);
    audioOut->initialize();
    _audioOuts[device.name] = audioOut;
    result = ma_device_start(playbackDevice.get());
    if (result != MA_SUCCESS) {
        std::cerr << "Failed to start playback device." << std::endl;
        ma_device_uninit(playbackDevice.get());
        return std::nullopt;
    }
    std::cout << "Started playback device: " << device.name << std::endl;
    _playbackDevices[device.name] = playbackDevice;
    _openedPlaybackDevices.push_back(device);
    return audioOut;
}

void flap::AudioManager::closePlaybackDevice(ma_device_info device) {
    std::lock_guard<std::mutex> lock(*_mutex);

    auto deviceIt = _playbackDevices.find(device.name);
    if (deviceIt == _playbackDevices.end()) {
        std::cerr << "Playback device not found: " << device.name << std::endl;
        return;
    }

    std::shared_ptr<ma_device> playbackDevice = deviceIt->second;

    // Stop the playback device if it is running
    ma_result result = ma_device_stop(playbackDevice.get());
    if (result != MA_SUCCESS) {
        std::cerr << "Failed to stop playback device: " << device.name << std::endl;
    } else {
        std::cout << "Stopped playback device: " << device.name << std::endl;
    }

    // Uninitialize the device to free resources
    ma_device_uninit(playbackDevice.get());
    std::cout << "Uninitialized playback device: " << device.name << std::endl;

    // Remove the device from the map
    _playbackDevices.erase(deviceIt);

    // Clean up associated data
    _playbackConfigs.erase(device.name);
    _audioCallbackDatas.erase(device.name);
    _audioOuts.erase(device.name);

    // Remove the device from the list of opened playback devices
    _openedPlaybackDevices.erase(std::remove_if(_openedPlaybackDevices.begin(), _openedPlaybackDevices.end(), [device](ma_device_info d) {
        return strcmp(d.name, device.name) == 0;
    }), _openedPlaybackDevices.end());

    std::cout << "Closed playback device: " << device.name << std::endl;
}

void flap::AudioManager::_threadFunction() {
    while (isRunning()) {
        {
            std::lock_guard<std::mutex> lock(*_mutex);
            ma_device_info* pPlaybackDevices;
            ma_uint32 playbackDeviceCount;
            ma_result result;
            result = ma_context_get_devices(&_playbackContext, &pPlaybackDevices, &playbackDeviceCount, NULL, NULL);
            if (result != MA_SUCCESS) {
                std::cerr << "Failed to enumerate playback devices." << std::endl;
                return;
            }
            _playbackDeviceInfos.clear();
            for (ma_uint32 i = 0; i < playbackDeviceCount; i++) {
                _playbackDeviceInfos.push_back(pPlaybackDevices[i]);
            }
            ma_device_info* pCaptureDevices;
            ma_uint32 captureDeviceCount;
            result = ma_context_get_devices(&_captureContext, NULL, NULL, &pCaptureDevices, &captureDeviceCount);
            if (result != MA_SUCCESS) {
                std::cerr << "Failed to enumerate capture devices." << std::endl;
                return;
            }
            _captureDeviceInfos.clear();
            for (ma_uint32 i = 0; i < captureDeviceCount; i++) {
                _captureDeviceInfos.push_back(pCaptureDevices[i]);
            }
        }
        std::this_thread::sleep_for(_updateInterval);
    }
}

void flap::AudioManager::_playbackDataCallback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount) {
    AudioCallbackData* callbackData = static_cast<AudioCallbackData*>(pDevice->pUserData);
    if (flap::AudioManager::getInstance()._audioOuts.find(callbackData->device.name) == flap::AudioManager::getInstance()._audioOuts.end()) {
        return;
    }
    auto a = flap::AudioManager::getInstance()._audioOuts[callbackData->device.name]->getAudioObjects()[0];
    auto sink = std::dynamic_pointer_cast<dibiff::sink::GraphSink>(a);
    if (sink == nullptr) {
        std::lock_guard<std::mutex> lock(sink->cv_mtx);
        sink->cv.notify_one();
        return;
    };
    float* outputBuffer = static_cast<float*>(pOutput);
    int channels = pDevice->playback.channels; // Number of output channels
    size_t samplesAvailable = sink->ringBuffers[0]->available();
    if (samplesAvailable == 0) {
        std::lock_guard<std::mutex> lock(sink->cv_mtx);
        sink->cv.notify_one();
        return;
    }
    // Read and interleave samples for each channel
    for (ma_uint32 frame = 0; frame < frameCount; ++frame) {
        for (int channel = 0; channel < channels; ++channel) {
            if (channel < sink->ringBuffers.size() && sink->ringBuffers[channel] != nullptr) {
                float sample;
                sink->ringBuffers[channel]->read(&sample, 1);
                outputBuffer[frame * channels + channel] = sample;
            } else {
                // If ring buffer for the channel is not available, write silence
                outputBuffer[frame * channels + channel] = 0.0f;
            }
        }
    }
    samplesAvailable = sink->ringBuffers[0]->available();
    // Signal that the callback has finished processing
    std::lock_guard<std::mutex> lock(sink->cv_mtx);
    sink->cv.notify_one();
}