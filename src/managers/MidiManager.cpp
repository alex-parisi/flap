/// MidiManager.cpp

#include "MidiManager.h"

#include <iostream>

bool MidiManager::initialize() {
    midiIn = std::make_shared<RtMidiIn>();
    // Check for available ports.
    unsigned int nPorts = midiIn->getPortCount();
    std::cout << "\n[" << nPorts << "] MIDI input source(s) available.\n";
    for (unsigned int i = 0; i < nPorts; i++) {
        std::string portName;
        try {
            portName = midiIn->getPortName(i);
        } catch (RtMidiError &error) {
            error.printMessage();
        }
        std::cout << "\tInput Port #" << i << ": " << portName << "\n";
    }
    // Open the specified port.
    try {
        midiIn->openPort(_port);
    } catch (RtMidiError &error) {
        std::cerr << error.getMessage() << "\n";
    }
    // Don't ignore sysex, timing, or active sensing messages.
    midiIn->ignoreTypes(false, false, false);
    // Set the callback function for MIDI input.
    midiIn->setCallback(&MidiManager::_midiCallback, this);

    return true;
}

void MidiManager::cleanup() {
    midiIn->closePort();
}

void MidiManager::_threadFunction() {
    while (isRunning()) {
        /// Mutex Locked
        {
            /// Update the list of available MIDI devices
            _portNames.clear();
            unsigned int nPorts = midiIn->getPortCount();
            for (unsigned int i = 0; i < nPorts; i++) {
                std::string portName;
                try {
                    portName = midiIn->getPortName(i);
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
        if (port < midiIn->getPortCount()) {
            midiIn->closePort();
            try {
                midiIn->openPort(port);
                // Don't ignore sysex, timing, or active sensing messages.
                midiIn->ignoreTypes(false, false, false);
                // Set the callback function for MIDI input.
                midiIn->setCallback(&MidiManager::_midiCallback, this);
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
    auto *midiInput = static_cast<MidiManager *>(userData);
    /// Mutex Locked
    {
        std::lock_guard<std::mutex> lock(static_cast<MidiManager *>(userData)->_mutex);
        if (message->size() > 0) {
            /// Push MIDI message to the audio graph
        }
    }
    /// Mutex Unlocked
}