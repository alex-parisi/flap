/// AudioManager.cpp

#define MINIAUDIO_IMPLEMENTATION
#include "AudioManager.h"
#include <iostream>

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
    ma_device_state state = ma_device_get_state(&_playbackDevice);
    if (state == ma_device_state_started) {
        ma_device_stop(&_playbackDevice);
        ma_device_uninit(&_playbackDevice);
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

std::optional<std::shared_ptr<flap::AudioOut>> flap::AudioManager::openPlaybackDevice(ma_device_info device, ma_format format, int channels, int sampleRate, int blockSize) {
    std::lock_guard<std::mutex> lock(*_mutex);

    ma_result result;
    /// TODO: Determine the supported formats and channels for the device
    /// TODO: Support stereo when selected 
    _playbackConfig = ma_device_config_init(ma_device_type_playback);
    _playbackConfig.playback.format = ma_format_f32;
    _playbackConfig.playback.channels = channels;
    _playbackConfig.sampleRate = sampleRate;
    _playbackConfig.dataCallback = _playbackDataCallback;
    _playbackConfig.pUserData = this;
    _playbackConfig.playback.pDeviceID = &device.id;
    _playbackConfig.periodSizeInFrames = blockSize;
    result = ma_device_init(&_playbackContext, &_playbackConfig, &_playbackDevice);
    if (result != MA_SUCCESS) {
        std::cerr << "Failed to initialize playback device." << std::endl;
        return std::nullopt;
    }
    _audioOut = std::make_shared<AudioOut>(0, channels, sampleRate, _settings->blockSize);
    _audioOut->initialize();
    result = ma_device_start(&_playbackDevice);
    if (result != MA_SUCCESS) {
        std::cerr << "Failed to start playback device." << std::endl;
        ma_device_uninit(&_playbackDevice);
        return std::nullopt;
    }
    return _audioOut;
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
    AudioManager* audioManager = static_cast<AudioManager*>(pDevice->pUserData);
    auto a = audioManager->_audioOut->getAudioObjects()[0];
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