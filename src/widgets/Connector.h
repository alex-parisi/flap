/// Connector.h

#include "dibiff/dibiff"
#include "imgui.h"
#include "imgui_internal.h"

#include <memory>
#include <string>
#include <optional>

#pragma once

namespace flap {
    class Connector : public std::enable_shared_from_this<Connector> {
        public:
            Connector() {};
            Connector(std::weak_ptr<dibiff::graph::AudioConnectionPoint> point, std::shared_ptr<dibiff::graph::AudioObject> object, bool allowMultiple = false) : _point(point), _object(object), _allowMultiple(allowMultiple) {};
            void render(std::optional<std::string> label = std::nullopt);
            std::weak_ptr<dibiff::graph::AudioConnectionPoint> getPoint() {
                return _point;
            }
            std::shared_ptr<dibiff::graph::AudioObject> getObject() {
                return _object;
            }
            void setSelected(bool selected) {
                _isSelected = selected;
            }
            void addConnectedTo(Connector* connectedTo) {
                _connectedTo.push_back(connectedTo);
            }
        private:
            std::weak_ptr<dibiff::graph::AudioConnectionPoint> _point;
            std::shared_ptr<dibiff::graph::AudioObject> _object;
            bool _isSelected = false;
            bool _allowMultiple;
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
            std::vector<Connector*> _connectedTo;
    };
}