/// AudioManager.cpp

#define MINIAUDIO_IMPLEMENTATION

#include "AudioManager.h"
#include <iostream>
#include <cstring>
#include "../MainApplicationSettings.h"

bool flap::AudioManager::initialize() {
    _mutex = std::make_unique<std::mutex>();
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
    for (auto & cd : _captureDevices) {
        ma_device_state state = ma_device_get_state(cd.second.get());
        if (state == ma_device_state_started) {
            ma_device_stop(cd.second.get());
            ma_device_uninit(cd.second.get());
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
    auto audioCallbackData = std::make_unique<AudioCallbackData>();
    audioCallbackData->device = device;
    _audioOutCallbackDatas[device.name] = std::move(audioCallbackData);
    /// TODO: Determine the supported formats and channels for the device
    /// TODO: Support stereo when selected 
    std::unique_ptr<ma_device> playbackDevice = std::make_unique<ma_device>();
    std::unique_ptr<ma_device_config> playbackConfig = std::make_unique<ma_device_config>();
    *playbackConfig = ma_device_config_init(ma_device_type_playback);
    playbackConfig->playback.format = ma_format_f32;
    playbackConfig->playback.channels = channels;
    playbackConfig->sampleRate = flap::MainApplicationSettingsManager::getInstance().settings.sampleRate;
    playbackConfig->dataCallback = _playbackDataCallback;
    playbackConfig->pUserData = _audioOutCallbackDatas[device.name].get();
    playbackConfig->playback.pDeviceID = &device.id;
    playbackConfig->periodSizeInFrames = flap::MainApplicationSettingsManager::getInstance().settings.blockSize;
    _playbackConfigs[device.name] = std::move(playbackConfig);
    result = ma_device_init(&_playbackContext, _playbackConfigs[device.name].get(), playbackDevice.get());
    if (result != MA_SUCCESS) {
        std::cerr << "Failed to initialize playback device." << std::endl;
        return std::nullopt;
    }
    std::cout << "Opened playback device: " << device.name << std::endl;
    std::shared_ptr<AudioOut> audioOut = std::make_shared<AudioOut>(device, channels, device.name);
    audioOut->initialize();
    _audioOuts[device.name] = audioOut.get();
    result = ma_device_start(playbackDevice.get());
    if (result != MA_SUCCESS) {
        std::cerr << "Failed to start playback device." << std::endl;
        ma_device_uninit(playbackDevice.get());
        return std::nullopt;
    }
    std::cout << "Started playback device: " << device.name << std::endl;
    _playbackDevices[device.name] = std::move(playbackDevice);
    _openedPlaybackDevices.push_back(device);
    return audioOut;
}

std::optional<std::shared_ptr<flap::AudioIn>> flap::AudioManager::openCaptureDevice(ma_device_info device, ma_format format, int channels) {
    std::lock_guard<std::mutex> lock(*_mutex);

    ma_result result;

    _audioInCallbackDatas[device.name] = std::make_unique<AudioCallbackData>();
    _captureDevices[device.name] = std::make_unique<ma_device>();
    _captureConfigs[device.name] = std::make_unique<ma_device_config>();
    std::shared_ptr<AudioIn> audioIn = std::make_shared<AudioIn>(device, channels, device.name);

    _audioInCallbackDatas[device.name]->device = device;

    /// TODO: Determine the supported formats and channels for the device
    /// TODO: Support stereo when selected 
    *_captureConfigs[device.name] = ma_device_config_init(ma_device_type_capture);
    _captureConfigs[device.name]->capture.format = ma_format_f32;
    _captureConfigs[device.name]->capture.channels = channels;
    _captureConfigs[device.name]->sampleRate = flap::MainApplicationSettingsManager::getInstance().settings.sampleRate;
    _captureConfigs[device.name]->dataCallback = _captureDataCallback;
    _captureConfigs[device.name]->pUserData = _audioInCallbackDatas[device.name].get();
    _captureConfigs[device.name]->capture.pDeviceID = &device.id;
    _captureConfigs[device.name]->periodSizeInFrames = flap::MainApplicationSettingsManager::getInstance().settings.blockSize;
    result = ma_device_init(&_captureContext, _captureConfigs[device.name].get(), _captureDevices[device.name].get());
    if (result != MA_SUCCESS) {
        std::cerr << "Failed to initialize capture device." << std::endl;
        return std::nullopt;
    }
    std::cout << "Opened capture device: " << device.name << std::endl;
    
    audioIn->initialize();
    _audioIns[device.name] = audioIn.get();
    result = ma_device_start(_captureDevices[device.name].get());
    if (result != MA_SUCCESS) {
        std::cerr << "Failed to start capture device." << std::endl;
        ma_device_uninit(_captureDevices[device.name].get());
        return std::nullopt;
    }

    std::cout << "Started capture device: " << device.name << std::endl;
    _openedCaptureDevices.push_back(device);
    return audioIn;
}

void flap::AudioManager::closePlaybackDevice(ma_device_info device) {
    std::lock_guard<std::mutex> lock(*_mutex);

    auto deviceIt = _playbackDevices.find(device.name);
    if (deviceIt == _playbackDevices.end()) {
        std::cerr << "Playback device not found: " << device.name << std::endl;
        return;
    }

    std::unique_ptr<ma_device> playbackDevice = std::move(deviceIt->second);

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
    _audioOutCallbackDatas.erase(device.name);
    _audioOuts.erase(device.name);

    // Remove the device from the list of opened playback devices
    _openedPlaybackDevices.erase(std::remove_if(_openedPlaybackDevices.begin(), _openedPlaybackDevices.end(), [device](ma_device_info d) {
        return strcmp(d.name, device.name) == 0;
    }), _openedPlaybackDevices.end());

    std::cout << "Closed playback device: " << device.name << std::endl;
}

void flap::AudioManager::closeCaptureDevice(ma_device_info device) {
    std::lock_guard<std::mutex> lock(*_mutex);

    auto deviceIt = _captureDevices.find(device.name);
    if (deviceIt == _captureDevices.end()) {
        std::cerr << "Capture device not found: " << device.name << std::endl;
        return;
    }

    std::unique_ptr<ma_device> captureDevice = std::move(deviceIt->second);

    // Stop the playback device if it is running
    ma_result result = ma_device_stop(captureDevice.get());
    if (result != MA_SUCCESS) {
        std::cerr << "Failed to stop capture device: " << device.name << std::endl;
    } else {
        std::cout << "Stopped capture device: " << device.name << std::endl;
    }

    // Uninitialize the device to free resources
    ma_device_uninit(captureDevice.get());
    std::cout << "Uninitialized capture device: " << device.name << std::endl;

    // Remove the device from the map
    _captureDevices.erase(deviceIt);

    // Clean up associated data
    _captureConfigs.erase(device.name);
    _audioInCallbackDatas.erase(device.name);
    _audioIns.erase(device.name);

    // Remove the device from the list of opened playback devices
    _openedCaptureDevices.erase(std::remove_if(_openedCaptureDevices.begin(), _openedCaptureDevices.end(), [device](ma_device_info d) {
        return strcmp(d.name, device.name) == 0;
    }), _openedCaptureDevices.end());

    std::cout << "Closed capture device: " << device.name << std::endl;
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
    if (a == nullptr) {
        return;
    }
    auto sink = static_cast<dibiff::sink::GraphSink*>(a);
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
                std::vector<float> sample = sink->ringBuffers[channel]->read(1);
                outputBuffer[frame * channels + channel] = sample[0];
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

void flap::AudioManager::_captureDataCallback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount) {
    AudioCallbackData* callbackData = static_cast<AudioCallbackData*>(pDevice->pUserData);
    if (flap::AudioManager::getInstance()._audioIns.find(callbackData->device.name) == flap::AudioManager::getInstance()._audioIns.end()) {
        return;
    }
    auto a = flap::AudioManager::getInstance()._audioIns[callbackData->device.name]->getAudioObjects()[0];
    auto source = static_cast<dibiff::source::GraphSource*>(a);
    if (source == nullptr) {
        std::lock_guard<std::mutex> lock(source->cv_mtx);
        source->cv.notify_one();
        return;
    };
    int channels = pDevice->capture.channels; // Number of output channels
    // Write interleaved samples for each channel, read from pInput
    const float* inputBuffer = static_cast<const float*>(pInput);
    for (ma_uint32 frame = 0; frame < frameCount; ++frame) {
        for (int channel = 0; channel < channels; ++channel) {
            if (channel < source->ringBuffers.size() && source->ringBuffers[channel] != nullptr) {
                std::vector<float> sample(1, inputBuffer[frame * channels + channel]);
                source->ringBuffers[channel]->write(sample, 1);
            }
        }
    }
    size_t samplesAvailable = source->ringBuffers[0]->available();
    // Signal that the callback has finished processing
    std::lock_guard<std::mutex> lock(source->cv_mtx);
    source->cv.notify_one();
}