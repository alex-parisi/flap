/// ConnectionService.h

#pragma once

#include <vector>
#include <tuple>
#include <memory>
#include "imgui.h"
#include "dibiff/dibiff"

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
            for (auto pair : _connections) {
                drawList->AddLine(std::get<0>(pair), std::get<1>(pair), IM_COL32(255, 255, 255, 255), 2.0f);
            }
        }
        void setDragging(bool dragging) {
            _dragging = dragging;
        }
        bool isDragging() {
            return _dragging;
        }
        void setActivePoint(std::weak_ptr<dibiff::graph::AudioConnectionPoint> point) {
            _activePoint = point;
        }
        std::weak_ptr<dibiff::graph::AudioConnectionPoint> getActivePoint() {
            return _activePoint;
        }
        void setActiveObject(std::shared_ptr<dibiff::graph::AudioObject> object) {
            _activeObject = object;
        }
        std::shared_ptr<dibiff::graph::AudioObject> getActiveObject() {
            return _activeObject;
        }
        void setMutex(std::shared_ptr<std::mutex> mutex) {
            _mutex = mutex;
        }
        std::shared_ptr<std::mutex> getMutex() {
            return _mutex;
        }
        void startDragging(ImVec2 centerPos, std::weak_ptr<dibiff::graph::AudioConnectionPoint> point, std::shared_ptr<dibiff::graph::AudioObject> object, bool& radioBool) {
            addPoint(centerPos);
            setDragging(true);
            setActivePoint(point);
            setActiveObject(object);
            _radioBool = &radioBool;
        }
        void stopDragging(ImVec2 centerpos, std::weak_ptr<dibiff::graph::AudioConnectionPoint> point, std::shared_ptr<dibiff::graph::AudioObject> object) {
            setDragging(false);
            /// Ensure we're not connecting this to itself
            if (getActiveObject() != object) {
                /// Try to connect the active point to the input of the gain object
                try {
                    std::lock_guard<std::mutex> lock(*getMutex());
                    dibiff::graph::AudioGraph::connect(getActivePoint(), point);
                    ConnectionService::getInstance().removePoint();
                    std::cout << "Connected\n";
                    *_radioBool = true;
                } catch (std::exception& e) {
                    /// If we can't connect, remove the last point
                    std::cout << "Can't connect\n";
                    ConnectionService::getInstance().removePoint();
                }
            } else {
                std::cout << "Can't connect to itself\n";
                ConnectionService::getInstance().removePoint();
            }
        }
    private:
        /// Singleton pattern
        ConnectionService() {}
        ~ConnectionService() {}
        /// List of points to draw lines between
        std::vector<ImVec2> _points;
        std::vector<std::tuple<ImVec2, ImVec2>> _connections;
        bool _dragging = false;
        std::weak_ptr<dibiff::graph::AudioConnectionPoint> _activePoint;
        std::shared_ptr<dibiff::graph::AudioObject> _activeObject;
        std::shared_ptr<std::mutex> _mutex;
        bool* _radioBool;
};