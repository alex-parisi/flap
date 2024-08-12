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
    /**
     * @brief MidiCallbackData is a struct that holds a pointer to the MidiManager and an id.
     */
    struct MidiCallbackData;
    /**
     * @brief MidiManager is a class that manages MIDI devices and MIDI data.
     */
    class MidiManager : public Manager {
        public: 
            /// Singleton pattern
            static MidiManager& getInstance() {
                static MidiManager instance;
                return instance;
            }
            /// Delete copy constructor and assignment operator to prevent copying
            MidiManager(const MidiManager&) = delete;
            MidiManager& operator=(const MidiManager&) = delete; 
            /**
             * @brief Initializes the MidiManager.
             */
            bool initialize() override;
            /**
             * @brief Cleans up the MidiManager.
             */
            void cleanup() override;
            /**
             * @brief Gets the input port names.
             * @return A vector of strings.
             */
            std::vector<std::string> getInputPortNames() { 
                std::lock_guard<std::mutex> lock(*_mutex);
                return _inputPortNames; 
            }
            /**
             * @brief Gets the output port names.
             * @return A vector of strings.
             */
            std::vector<std::string> getOutputPortNames() { 
                std::lock_guard<std::mutex> lock(*_mutex);
                return _outputPortNames; 
            }
            /**
             * @brief Opens an input port.
             * @param port The port to open.
             * @return A shared pointer to a MidiIn object.
             */
            std::optional<std::shared_ptr<MidiIn>> openInputPort(int port);
            /**
             * @brief Opens an output port.
             * @param port The port to open.
             * @return A shared pointer to a MidiOut object.
             */
            /// TODO: Implement this
            bool openOutputPort(int port) { return false;};
            /**
             * @brief Gets the input port name.
             * @param port The port to get the name of.
             * @return A string.
             */
            std::string getInPortName(int port) {
                std::lock_guard<std::mutex> lock(*_mutex);
                return _mainMidiIn->getPortName(port);
            }
            /**
             * @brief Gets the output port name.
             * @param port The port to get the name of.
             * @return A string.
             */
            std::string getOutPortName(int port) {
                std::lock_guard<std::mutex> lock(*_mutex);
                return _mainMidiOut->getPortName(port);
            }
        private:
            /// Singleton pattern
            MidiManager() {}
            ~MidiManager() {}
            /**
             * @brief A map of RtMidiIn contexts
             */
            std::map<int, std::shared_ptr<RtMidiIn>> _midiIns;
            /**
             * @brief A map of MidiIn objects
             */
            std::map<int, std::shared_ptr<MidiIn>> _midiInObjects;
            /**
             * @brief A map of RtMidiOut contexts
             */
            std::map<int, std::shared_ptr<RtMidiOut>> _midiOuts;
            /**
             * @brief A map of MIDI callback data
             */
            std::map<int, std::shared_ptr<MidiCallbackData>> callbackDataMap;
            /**
             * @brief The main RtMidiIn context
             */
            std::shared_ptr<RtMidiIn> _mainMidiIn;
            /**
             * @brief The main RtMidiOut context
             */
            std::shared_ptr<RtMidiOut> _mainMidiOut;
            /**
             * @brief The input port names.
             */
            std::vector<std::string> _inputPortNames;
            /**
             * @brief The output port names.
             */
            std::vector<std::string> _outputPortNames;
            /**
             * @brief The update interval.
             */
            std::chrono::duration<int, std::milli> _updateInterval = std::chrono::milliseconds(1000);
            /**
             * @brief The thread function for the MidiManager.
             */
            void _threadFunction() override;
            /**
             * @brief The MIDI callback.
             */
            static void _midiCallback(double deltatime, std::vector<unsigned char> *message, void *userData);
    };
    /**
     * @brief MidiCallbackData is a struct that holds a pointer to the MidiManager and an id.
     */
    struct MidiCallbackData {
        /**
         * @brief MidiCallbackData constructor.
         */
        flap::MidiManager* manager;
        /**
         * @brief The id.
         */
        int port;
    };
}