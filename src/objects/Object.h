/// Object.h

#pragma once

#include <vector>

#include "imgui.h"
#include "dibiff/dibiff"

namespace flap {
    class Object : public std::enable_shared_from_this<Object> {
        public:
            virtual void initialize() = 0;
            virtual void render() = 0;
            std::vector<std::shared_ptr<dibiff::graph::AudioObject>> getAudioObjects() { return _audioObjects; }
        protected:
            std::vector<std::shared_ptr<dibiff::graph::AudioObject>> _audioObjects;
            /// @brief  Get the center position of a radio button. Note that this MUST
            ///         be called after the radio button has been rendered via:
            ///             if (ImGui::RadioButton("MIDI Input", false)) {
            static ImVec2 _getRadioButtonCenter() {
                // Get the starting position of the radio button
                ImVec2 rectMin = ImGui::GetItemRectMin();
                // Estimate the size of the radio button
                float radioButtonSize = ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.y * 2;
                // Calculate the center position of the radio button
                return ImVec2(rectMin.x + radioButtonSize / 2, rectMin.y + radioButtonSize / 2);
            }
    };
}