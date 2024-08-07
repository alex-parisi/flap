/// Connector.cpp

#include "Connector.h"
#include "../ConnectionService.h"

void flap::Connector::render(std::optional<std::string> label) {
    std::string title = (label.has_value()) ? label.value() : _point.lock()->getName();
    if (ImGui::RadioButton(title.c_str(), _isSelected)) {
        if (!_allowMultiple) {
            if (!_isSelected) {
                if (!ConnectionService::getInstance().isDragging()) {
                    ConnectionService::getInstance().startDragging(_getRadioButtonCenter(), _point, _object, _isSelected);
                } else {
                    ConnectionService::getInstance().stopDragging(_getRadioButtonCenter(), _point, _object);
                    _isSelected = true;
                }
            } else {
                /// TODO: Disconnect 
            }
        } else {
            if (!ConnectionService::getInstance().isDragging()) {
                ConnectionService::getInstance().startDragging(_getRadioButtonCenter(), _point, _object, _isSelected);
            } else {
                ConnectionService::getInstance().stopDragging(_getRadioButtonCenter(), _point, _object);
                _isSelected = true;
            }
        }
    }
    /// Update the point location
    if (_isSelected) {
        ConnectionService::getInstance().getConnectionLocations()[_point] = _getRadioButtonCenter();
    }
}