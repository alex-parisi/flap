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
    /**
     * @brief AudioCallbackData is a struct that holds a pointer to the AudioManager and an id.
     */
    struct AudioCallbackData;
    /**
     * @brief AudioManager is a class that manages audio devices and audio data.
     */
    class AudioManager : public Manager {
        public: 
            /**
             * @brief AudioManager constructor.
             * @param settings The settings for the main application.
             */
            AudioManager(std::shared_ptr<MainApplicationSettings> settings) : Manager(settings) {}
            /**
             * @brief Initializes the AudioManager.
             */
            bool initialize() override;
            /**
             * @brief Cleans up the AudioManager.
             */
            void cleanup() override;
            /**
             * @brief Gets the playback devices.
             * @return A vector of ma_device_info.
             */
            std::vector<ma_device_info> getPlaybackDevices();
            /**
             * @brief Gets the capture devices.
             * @return A vector of ma_device_info.
             */
            std::vector<ma_device_info> getCaptureDevices();
            /**
             * @brief Opens a playback device.
             * @param device The device to open.
             * @param format The format of the device.
             * @param channels The number of channels.
             * @param sampleRate The sample rate.
             * @param blockSize The block size.
             * @return A shared pointer to an AudioOut object.
             */
            std::optional<std::shared_ptr<AudioOut>> openPlaybackDevice(ma_device_info device, ma_format format, int channels, int sampleRate, int blockSize);
        private:
            /**
             * @brief The playback context.
             */
            ma_context _playbackContext;
            /**
             * @brief The capture context.
             */
            ma_context _captureContext;
            /**
             * @brief The playback device infos.
             */
            std::vector<ma_device_info> _playbackDeviceInfos;
            /**
             * @brief The capture device infos.
             */
            std::vector<ma_device_info> _captureDeviceInfos;
            /**
             * @brief The playback device configs.
             */
            ma_device_config _playbackConfig;
            /**
             * @brief The playback device.
             */
            ma_device _playbackDevice;
            /**
             * @brief The AudioOut object associated with the playback device.
             */
            std::shared_ptr<AudioOut> _audioOut;
            /**
             * @brief The rate at which the AudioManager updates.
             */
            std::chrono::duration<int, std::milli> _updateInterval = std::chrono::milliseconds(1000);
            /**
             * @brief The thread function for the AudioManager.
             */
            void _threadFunction() override;
            /**
             * @brief The playback data callback.
             */
            static void _playbackDataCallback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount);
    };
    /**
     * @brief AudioCallbackData is a struct that holds a pointer to the AudioManager and an id.
     */
    struct AudioCallbackData {
        /**
         * @brief The AudioManager pointer.
         */
        flap::AudioManager* manager;
        /**
         * @brief The id.
         */
        int id;
    };
}