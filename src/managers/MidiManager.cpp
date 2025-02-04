/// MidiManager.cpp

#include "MidiManager.h"
#include "../objects/io/MidiIn.h"
#include "../managers/ObjectManager.h"
#include "../managers/GraphManager.h"

#include <iostream>

bool flap::MidiManager::initialize() {
    _mutex = std::make_unique<std::mutex>();
    _mainMidiIn = std::make_unique<RtMidiIn>();
    _mainMidiOut = std::make_unique<RtMidiOut>();
    return true;
}

void flap::MidiManager::cleanup() {
    _mainMidiIn->closePort();
    _mainMidiOut->closePort();
    _mainMidiIn.reset();
    _mainMidiOut.reset();
    _midiIns.clear();
    _midiInObjects.clear();
}

std::unique_ptr<flap::MidiIn> flap::MidiManager::openInputPort(int port) {
    /// Mutex Locked
    {
        std::lock_guard<std::mutex> lock(*_mutex);
        /// Make a new RtMidiIn object
        auto newMidiIn = std::make_unique<RtMidiIn>();
        if (port < newMidiIn->getPortCount()) {
            newMidiIn->closePort();
            try {
                newMidiIn->openPort(port);
                auto newMidiInObject = std::make_unique<flap::MidiIn>(port, newMidiIn->getPortName(port));
                newMidiInObject->initialize();
                /// Don't ignore sysex, timing, or active sensing messages.
                newMidiIn->ignoreTypes(false, false, false);
                /// Set the callback function for MIDI input.
                auto callbackData = std::make_unique<MidiCallbackData>(MidiCallbackData{this, port});
                newMidiIn->setCallback(&MidiManager::_midiCallback, callbackData.get());
                _midiIns[port] = std::move(newMidiIn);
                _midiInObjects[port] = newMidiInObject.get();
                callbackDataMap[port] = std::move(callbackData);
                return std::move(newMidiInObject);
            } catch (RtMidiError &error) {
                std::cerr << error.getMessage() << "\n";
                return nullptr;
            }        
        }   
    }
    return nullptr;
}

std::unique_ptr<flap::KeyboardSimulator> flap::MidiManager::createSimulator() {
    /// Mutex Locked
    {
        std::lock_guard<std::mutex> lock(*_mutex);
        auto keyboardSim = std::make_unique<flap::KeyboardSimulator>("KeyboardSimulator" + std::to_string(flap::ObjectManager::getInstance().keyboardSimulatorCounter++));
        keyboardSim->initialize();
        _simulators.push_back(keyboardSim.get());
        return std::move(keyboardSim);
    }
}

void flap::MidiManager::_threadFunction() {
    while (isRunning()) {
        /// Mutex Locked
        {
            std::lock_guard<std::mutex> lock(*_mutex);
            /// Update the list of available MIDI devices
            _inputPortNames.clear();
            unsigned int nPorts = _mainMidiIn->getPortCount();
            for (unsigned int i = 0; i < nPorts; i++) {
                std::string portName;
                try {
                    portName = _mainMidiIn->getPortName(i);
                } catch (RtMidiError &error) {
                    error.printMessage();
                }
                _inputPortNames.push_back("[" + std::to_string(i) + "] " + portName);
            }
            _outputPortNames.clear();
            nPorts = _mainMidiOut->getPortCount();
            for (unsigned int i = 0; i < nPorts; i++) {
                std::string portName;
                try {
                    portName = _mainMidiOut->getPortName(i);
                } catch (RtMidiError &error) {
                    error.printMessage();
                }
                _outputPortNames.push_back("[" + std::to_string(i) + "] " + portName);
            }
        }
        /// Mutex Unlocked
        /// Sleep for the update interval
        std::this_thread::sleep_for(_updateInterval);
    }
}

void flap::MidiManager::_midiCallback(double deltatime, std::vector<unsigned char> *message, void *userData) {
    auto callbackData = static_cast<MidiCallbackData*>(userData);
    auto manager = callbackData->manager;
    int port = callbackData->port;
    /// Mutex Locked
    {
        std::lock_guard<std::mutex> lock(*manager->_mutex);
        if (message->size() > 0) {
            /// Push MIDI message to the audio graph
            /// TODO: Search for the correct audio object, don't assume it's the first one
            std::vector<std::vector<unsigned char>> messages = { *message };
            auto a = manager->_midiInObjects[port]->getAudioObjects()[0];
            auto b = static_cast<dibiff::midi::MidiInput*>(a);
            b->addMidiMessage(messages);
        }
    }
    /// Mutex Unlocked
}