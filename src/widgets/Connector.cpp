/// Connector.cpp

#include "Connector.h"
#include "../ConnectionService.h"
#include "dibiff/dibiff"

void flap::Connector::render(std::optional<std::string> label) {
    std::string title = (label.has_value()) ? label.value() : _point.lock()->getName();
    
    // Render the radio button
    bool _isSelected = _point.lock()->isConnected();
    if (ImGui::RadioButton(title.c_str(), _isSelected)) {
        if (!_allowMultiple) {
            if (!_isSelected) {
                if (!ConnectionService::getInstance().isDragging()) {
                    ConnectionService::getInstance().startDragging(_getRadioButtonCenter(), *this);
                } else {
                    ConnectionService::getInstance().stopDragging(_getRadioButtonCenter(), this);
                    _connectedTo.push_back(ConnectionService::getInstance().getCurrentConnector());
                }
            }
        } else {
            if (!ConnectionService::getInstance().isDragging()) {
                ConnectionService::getInstance().startDragging(_getRadioButtonCenter(), *this);
            } else {
                ConnectionService::getInstance().stopDragging(_getRadioButtonCenter(), this);
                _connectedTo.push_back(ConnectionService::getInstance().getCurrentConnector());
            }
        }
    }

    // Check if the item is hovered and the right mouse button is clicked
    if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
        /// TODO: Disconnect
        /// First, disconnect the audio graph via dibiff::graph::AudioGraph::disconnect
        /// Then, remove the connection from the ConnectionService
        /// Finally, remove the connection from the _connectedTo vector
        for (auto& connection : _connectedTo) {
            dibiff::graph::AudioGraph::disconnect(_point, connection->getPoint());
            ConnectionService::getInstance().removeConnection(_point, connection->getPoint());
        }
        _connectedTo.clear();
    }

    // Update the point location
    if (_isSelected) {
        ConnectionService::getInstance().getConnectionLocations()[_point] = _getRadioButtonCenter();
    }
}