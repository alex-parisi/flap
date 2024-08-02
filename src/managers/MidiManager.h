/// MidiManager.h

#include "Manager.h"
#include "RtMidi.h"

class MidiManager : public Manager {
    public: 
        MidiManager(int port = 0) : Manager(), _port(port) {}
        bool initialize() override;
        void cleanup() override;
        std::vector<std::string> getPortNames() { return _portNames; }
        bool setPort(int port);
    private:
        int _port;
        std::shared_ptr<RtMidiIn> midiIn;
        std::vector<std::string> _portNames;
        std::chrono::duration<int, std::milli> _updateInterval = std::chrono::milliseconds(1000);
        void _threadFunction() override;
        static void _midiCallback(double deltatime, std::vector<unsigned char> *message, void *userData);
};