/// MidiManager.cpp

#include "MidiManager.h"

#include <iostream>

bool MidiManager::initialize() {
    _midiIn = std::make_shared<RtMidiIn>();
    _midiGraphInput = dibiff::midi::MidiInput::create(_blockSize);
    // Check for available ports.
    unsigned int nPorts = _midiIn->getPortCount();
    std::cout << "\n[" << nPorts << "] MIDI input source(s) available.\n";
    for (unsigned int i = 0; i < nPorts; i++) {
        std::string portName;
        try {
            portName = _midiIn->getPortName(i);
        } catch (RtMidiError &error) {
            error.printMessage();
        }
        std::cout << "\tInput Port #" << i << ": " << portName << "\n";
    }
    // Open the specified port.
    try {
        _midiIn->openPort(_port);
    } catch (RtMidiError &error) {
        std::cerr << error.getMessage() << "\n";
    }
    // Don't ignore sysex, timing, or active sensing messages.
    _midiIn->ignoreTypes(false, false, false);
    // Set the callback function for MIDI input.
    _midiIn->setCallback(&MidiManager::_midiCallback, this);
    return true;
}

void MidiManager::cleanup() {
    _midiIn->closePort();
}

void MidiManager::_threadFunction() {
    while (isRunning()) {
        /// Mutex Locked
        {
            /// Update the list of available MIDI devices
            _portNames.clear();
            unsigned int nPorts = _midiIn->getPortCount();
            for (unsigned int i = 0; i < nPorts; i++) {
                std::string portName;
                try {
                    portName = _midiIn->getPortName(i);
                } catch (RtMidiError &error) {
                    error.printMessage();
                }
                _portNames.push_back("[" + std::to_string(i) + "] " + portName);
            }
        }
        /// Mutex Unlocked
        /// Sleep for the update interval
        std::this_thread::sleep_for(_updateInterval);
    }
}

bool MidiManager::setPort(int port) {
    /// Mutex Locked
    {
        std::lock_guard<std::mutex> lock(_mutex);
        if (port < _midiIn->getPortCount()) {
            _midiIn->closePort();
            try {
                _midiIn->openPort(port);
                // Don't ignore sysex, timing, or active sensing messages.
                _midiIn->ignoreTypes(false, false, false);
                // Set the callback function for MIDI input.
                _midiIn->setCallback(&MidiManager::_midiCallback, this);
                _port = port;
                return true;
            } catch (RtMidiError &error) {
                std::cerr << error.getMessage() << "\n";
                return false;
            }        
        }
    }
    return false;
}

void MidiManager::_midiCallback(double deltatime, std::vector<unsigned char> *message, void *userData) {
    auto manager = static_cast<MidiManager *>(userData);
    /// Mutex Locked
    {
        std::lock_guard<std::mutex> lock(manager->_mutex);
        if (message->size() > 0) {
            /// Push MIDI message to the audio graph
            manager->_midiGraphInput->addMidiMessage(*message);
        }
    }
    /// Mutex Unlocked
}