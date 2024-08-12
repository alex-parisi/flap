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

// Custom hash function for std::weak_ptr
struct WeakPtrHash {
    template <typename T>
    std::size_t operator()(const std::weak_ptr<T>& wp) const {
        auto sp = wp.lock();
        return std::hash<std::shared_ptr<T>>()(sp);
    }
};

// Custom equality function for std::weak_ptr
struct WeakPtrEqual {
    template <typename T>
    bool operator()(const std::weak_ptr<T>& lhs, const std::weak_ptr<T>& rhs) const {
        return !lhs.owner_before(rhs) && !rhs.owner_before(lhs);
    }
};

class ConnectionService {
    public:
        /// Singleton pattern
        static ConnectionService& getInstance() {
            static ConnectionService instance; // Guaranteed to be destroyed and instantiated on first use
            return instance;
        }
        /// Delete copy constructor and assignment operator to prevent copying
        ConnectionService(const ConnectionService&) = delete;
        ConnectionService& operator=(const ConnectionService&) = delete;

        /// Method to add a point
        void addPoint(const ImVec2& point) {
            _points.push_back(point);
        }
        void removePoint() {
            _points.pop_back();
        }
        /// Method to render the connections
        void renderConnections() {
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
        void setDragging(bool dragging) {
            _dragging = dragging;
        }
        bool isDragging() {
            return _dragging;
        }
        void setMutex(std::shared_ptr<std::recursive_mutex> mutex) {
            _mutex = mutex;
        }
        std::shared_ptr<std::recursive_mutex> getMutex() {
            return _mutex;
        }
        void startDragging(ImVec2 centerPos, flap::Connector& firstConnector) {
            addPoint(centerPos);
            setDragging(true);
            _currentConnector = &firstConnector;
        }
        void stopDragging(ImVec2 centerpos, flap::Connector* secondConnector) {
            setDragging(false);
            /// Ensure we're not connecting this to itself
            if (_currentConnector->getObject() != secondConnector->getObject()) {
                /// Try to connect the active point to the input of the gain object
                try {
                    std::lock_guard<std::recursive_mutex> lock(*getMutex());
                    dibiff::graph::AudioGraph::connect(_currentConnector->getPoint(), secondConnector->getPoint());
                    std::cout << "Connected\n";
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
        std::unordered_map<std::weak_ptr<dibiff::graph::AudioConnectionPoint>, ImVec2, WeakPtrHash, WeakPtrEqual>& getConnectionLocations() {
            return _connectionLocations;
        }
        flap::Connector* getCurrentConnector() {
            return _currentConnector;
        }
        void removeConnection(std::weak_ptr<dibiff::graph::AudioConnectionPoint> point1, std::weak_ptr<dibiff::graph::AudioConnectionPoint> point2) {
            _connections.erase(std::remove_if(_connections.begin(), _connections.end(), 
                [&](const auto& pair) {
                    auto p1 = std::get<0>(pair).lock();
                    auto p2 = std::get<1>(pair).lock();
                    return p1 == point1.lock() && p2 == point2.lock() || p1 == point2.lock() && p2 == point1.lock();
                }), _connections.end());
        }
        void removeConnection(std::weak_ptr<dibiff::graph::AudioConnectionPoint> point) {
            _connections.erase(std::remove_if(_connections.begin(), _connections.end(), 
                [&](const auto& pair) {
                    auto p1 = std::get<0>(pair).lock();
                    auto p2 = std::get<1>(pair).lock();
                    return p1 == point.lock() || p2 == point.lock();
                }), _connections.end());
        }
        void setShowConnections(bool showConnections) {
            _showConnections = showConnections;
        }
        bool getShowConnections() {
            return _showConnections;
        }
    private:
        /// Singleton pattern
        ConnectionService() {}
        ~ConnectionService() {}
        /// List of points to draw lines between
        std::vector<ImVec2> _points;
        bool _dragging = false;
        std::shared_ptr<std::recursive_mutex> _mutex;
        flap::Connector* _currentConnector;
        std::unordered_map<std::weak_ptr<dibiff::graph::AudioConnectionPoint>, ImVec2, WeakPtrHash, WeakPtrEqual> _connectionLocations;
        std::vector<std::tuple<std::weak_ptr<dibiff::graph::AudioConnectionPoint>, std::weak_ptr<dibiff::graph::AudioConnectionPoint>>> _connections;
        bool _showConnections = true;
};