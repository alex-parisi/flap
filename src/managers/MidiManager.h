/// MidiManager.h

#pragma once

#include <vector>
#include <string>
#include <chrono>
#include <memory>
#include <map>
#include <mutex>
#include <optional>

#include "Manager.h"
#include "RtMidi.h"
#include "dibiff/dibiff"
#include "../objects/io/MidiIn.h"

namespace flap {
    struct MidiCallbackData;
    class MidiManager : public Manager {
        public: 
            MidiManager(std::shared_ptr<MainApplicationSettings> settings) : Manager(settings) {}
            bool initialize() override;
            void cleanup() override;
            std::vector<std::string> getInputPortNames() { 
                std::lock_guard<std::mutex> lock(_mutex);
                return _inputPortNames; 
            }
            std::vector<std::string> getOutputPortNames() { 
                std::lock_guard<std::mutex> lock(_mutex);
                return _outputPortNames; 
            }
            std::optional<std::shared_ptr<MidiIn>> openInputPort(int port);
            /// TODO: Implement this
            bool openOutputPort(int port) { return false;};
            std::string getInPortName(int port) {
                std::lock_guard<std::mutex> lock(_mutex);
                return _mainMidiIn->getPortName(port);
            }
            std::string getOutPortName(int port) {
                std::lock_guard<std::mutex> lock(_mutex);
                return _mainMidiOut->getPortName(port);
            }
        private:
            std::map<int, std::shared_ptr<RtMidiIn>> _midiIns;
            std::map<int, std::shared_ptr<MidiIn>> _midiInObjects;
            std::map<int, std::shared_ptr<RtMidiOut>> _midiOuts;
            std::map<int, std::shared_ptr<MidiCallbackData>> callbackDataMap;
            std::shared_ptr<RtMidiIn> _mainMidiIn;
            std::shared_ptr<RtMidiOut> _mainMidiOut;
            std::vector<std::string> _inputPortNames;
            std::vector<std::string> _outputPortNames;
            std::chrono::duration<int, std::milli> _updateInterval = std::chrono::milliseconds(1000);
            void _threadFunction() override;
            static void _midiCallback(double deltatime, std::vector<unsigned char> *message, void *userData);
    };
    struct MidiCallbackData {
        flap::MidiManager* manager;
        int port;
    };
}