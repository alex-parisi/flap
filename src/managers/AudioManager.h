/// AudioManager.h

#include "Manager.h"

#include <condition_variable>
#include <mutex>

#include "miniaudio.h"
#include "dibiff/dibiff"

class AudioManager : public Manager {
    public: 
        AudioManager(int sampleRate = 48000, int blockSize = 512) : Manager(), _sampleRate(sampleRate), _blockSize(blockSize) {}
        bool initialize() override;
        void cleanup() override;
        std::shared_ptr<dibiff::sink::GraphSink> getGraphSink() { return _graphSink; }
        bool selectPlaybackDevice(int index);
        bool selectCaptureDevice(int index);
    private:
        int _sampleRate;
        int _blockSize;
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