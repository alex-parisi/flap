/// Object.cpp

#include "Object.h"

#include "../managers/GraphManager.h"
#include "../managers/ObjectManager.h"
#include "../ConnectionService.h"

#include <mutex>

flap::Object::Object() : _isOpen([&, this]() {
    /// For each object in _audioObjects, disconnect all connections then remove it from the graph
    preDisconnect();
    {
        std::lock_guard<std::mutex> lock(flap::GraphManager::getInstance().getMutex());
        for (auto& audioObject : _audioObjects) {
            audioObject->disconnectAll();
            flap::GraphManager::getInstance().removeObject(audioObject.get());
        }
        for (auto& obj : _audioObjects) {
            for (auto& i : obj->_inputs) {
                ConnectionService::getInstance().removeConnection(i.get());
            }
            for (auto& o : obj->_outputs) {
                ConnectionService::getInstance().removeConnection(o.get());
            }
        }
        /// Clear the _audioObjects vector
        _audioObjects.clear();
    }
    postDisconnect();
    /// Remove this object from the flap::MainApplication::getInstance().objects vector
    shouldDelete = true;
}) {}