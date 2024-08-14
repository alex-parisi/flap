/// KeyboardSimulator.h

#pragma once

#include "../Object.h"
#include "../../widgets/Connector.h"

#include <memory>
#include <optional>
#include <string>
#include <map>

namespace flap {
    class KeyboardSimulator : public Object {
    public:
        inline KeyboardSimulator(std::optional<std::string> name = std::nullopt) : Object(), _name(name) {}
        void initialize() override;
        void render() override;
        void update();
    private:
        Connector _output;
        std::optional<std::string> _name;
        std::map<int, int> _midiNotes = {
            { ImGuiKey_A, 60 }, // Middle C
            { ImGuiKey_W, 61 }, // C#
            { ImGuiKey_S, 62 }, // D
            { ImGuiKey_E, 63 }, // D#
            { ImGuiKey_D, 64 }, // E
            { ImGuiKey_F, 65 }, // F
            { ImGuiKey_T, 66 }, // F#
            { ImGuiKey_G, 67 }, // G
            { ImGuiKey_Y, 68 }, // G#
            { ImGuiKey_H, 69 }, // A
            { ImGuiKey_U, 70 }, // A#
            { ImGuiKey_J, 71 }, // B
            { ImGuiKey_K, 72 }  // High C
        };
    };
}