/// Object.cpp

#include "Object.h"

#include "../managers/GraphManager.h"
#include "../managers/ObjectManager.h"
#include "../ConnectionService.h"

#include <mutex>

flap::Object::Object() : _isOpen([&, this]() {
    /// For each object in _audioObjects, disconnect all connections then remove it from the graph
    {
        /// TODO: Disconnect the other objects from this object as well
        std::cout << "Locking mutex" << std::endl;
        std::lock_guard<std::recursive_mutex> lock(*flap::GraphManager::getInstance().getMutex());
        for (auto& audioObject : _audioObjects) {
            audioObject->disconnectAll();
            flap::GraphManager::getInstance().removeObject(audioObject);
        }
        std::cout << "Object removed" << std::endl;
        for (auto& obj : _audioObjects) {
            for (auto& i : obj->_inputs) {
                ConnectionService::getInstance().removeConnection(i);
            }
            for (auto& o : obj->_outputs) {
                ConnectionService::getInstance().removeConnection(o);
            }
        }
        std::cout << "Connections removed" << std::endl;
        /// Clear the _audioObjects vector
        _audioObjects.clear();
        std::cout << "Object cleared" << std::endl;
    }
    /// Remove this object from the flap::MainApplication::getInstance().objects vector
    shouldDelete = true;
}) {}