/// ConnectionService.h

#pragma once

#include <vector>
#include <tuple>
#include <memory>
#include <unordered_map>
#include <mutex>
#include "imgui.h"
#include "dibiff/dibiff"
#include "widgets/Connector.h"
#include "managers/GraphManager.h"

class ConnectionService {
    public:
        /// Singleton pattern
        inline static ConnectionService& getInstance() {
            static ConnectionService instance; // Guaranteed to be destroyed and instantiated on first use
            return instance;
        }
        /// Delete copy constructor and assignment operator to prevent copying
        ConnectionService(const ConnectionService&) = delete;
        ConnectionService& operator=(const ConnectionService&) = delete;

        /// Method to add a point
        inline void addPoint(const ImVec2& point) {
            _points.push_back(point);
        }
        inline void removePoint() {
            _points.pop_back();
        }
        /// Method to render the connections
        inline void renderConnections() {
            ImDrawList* drawList = ImGui::GetForegroundDrawList();
            /// Add the current mouse position to the list of points
            std::vector<ImVec2> p(_points.begin(), _points.end());
            p.push_back(ImGui::GetMousePos());
            for (size_t i = 1; i < p.size(); i += 2) {
                drawList->AddLine(p[i - 1], p[i], IM_COL32(255, 255, 255, 255), 2.0f);
            }
            if (_showConnections) {
                for (auto pair : _connections) {
                    ImVec2 pt1 = _connectionLocations[std::get<0>(pair)];
                    ImVec2 pt2 = _connectionLocations[std::get<1>(pair)];
                    drawList->AddLine(pt1, pt2, IM_COL32(255, 255, 255, 255), 1.0f);
                }
            }
        }
        inline void setDragging(bool dragging) {
            _dragging = dragging;
        }
        inline bool isDragging() {
            return _dragging;
        }
        inline void startDragging(ImVec2 centerPos, flap::Connector* firstConnector) {
            addPoint(centerPos);
            setDragging(true);
            _currentConnector = firstConnector;
        }
        inline void stopDragging(ImVec2 centerpos, flap::Connector* secondConnector) {
            setDragging(false);
            /// Ensure we're not connecting this to itself
            if (_currentConnector->getObject() != secondConnector->getObject()) {
                /// Try to connect the active point to the input of the gain object
                try {
                    // std::lock_guard<std::mutex> lock(flap::GraphManager::getInstance().getMutex());
                    dibiff::graph::AudioGraph::connect(_currentConnector->getPoint(), secondConnector->getPoint());
                    std::cout << "Connected " << _currentConnector->getPoint()->getName() << " to " << secondConnector->getPoint()->getName() << "\n";
                    _connectionLocations[secondConnector->getPoint()] = centerpos;
                    _connectionLocations[_currentConnector->getPoint()] = _points.back();
                    _connections.push_back(std::make_tuple(_currentConnector->getPoint(), secondConnector->getPoint()));
                    _currentConnector->addConnectedTo(secondConnector);
                    removePoint();
                } catch (std::exception& e) {
                    /// If we can't connect, remove the last point
                    std::cout << "Can't connect\n";
                    removePoint();
                }
            } else {
                std::cout << "Can't connect to itself\n";
                removePoint();
            }
        }
        inline std::unordered_map<dibiff::graph::AudioConnectionPoint*, ImVec2>& getConnectionLocations() {
            return _connectionLocations;
        }
        inline flap::Connector* getCurrentConnector() {
            return _currentConnector;
        }
        inline void removeConnection(dibiff::graph::AudioConnectionPoint* point1, dibiff::graph::AudioConnectionPoint* point2) {
            _connections.erase(std::remove_if(_connections.begin(), _connections.end(), 
                [&](const std::tuple<dibiff::graph::AudioConnectionPoint*, dibiff::graph::AudioConnectionPoint*>& pair) {
                    auto p1 = std::get<0>(pair);
                    auto p2 = std::get<1>(pair);
                    return p1 == point1 && p2 == point2 || p1 == point2 && p2 == point1;
                }), _connections.end());
        }
        inline void removeConnection(dibiff::graph::AudioConnectionPoint* point) {
            _connections.erase(std::remove_if(_connections.begin(), _connections.end(), 
                [&](const std::tuple<dibiff::graph::AudioConnectionPoint*, dibiff::graph::AudioConnectionPoint*>& pair) {
                    auto p1 = std::get<0>(pair);
                    auto p2 = std::get<1>(pair);
                    return p1 == point || p2 == point;
                }), _connections.end());
        }
        inline void setShowConnections(bool showConnections) {
            _showConnections = showConnections;
        }
        inline bool getShowConnections() {
            return _showConnections;
        }
    private:
        /// Singleton pattern
        inline ConnectionService() {}
        inline ~ConnectionService() {}
        /// List of points to draw lines between
        std::vector<ImVec2> _points;
        bool _dragging = false;
        flap::Connector* _currentConnector;
        std::unordered_map<dibiff::graph::AudioConnectionPoint*, ImVec2> _connectionLocations;
        std::vector<std::tuple<dibiff::graph::AudioConnectionPoint*, dibiff::graph::AudioConnectionPoint*>> _connections;
        bool _showConnections = true;
};