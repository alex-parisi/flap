/// GraphManager.cpp

#include "GraphManager.h"

bool flap::GraphManager::initialize() {
    _graph = std::make_shared<dibiff::graph::AudioGraph>();
    return true;
}

void flap::GraphManager::cleanup() {
}

void flap::GraphManager::addObject(std::shared_ptr<dibiff::graph::AudioObject> object) { 
    /// Mutex Locked
    {
        // std::lock_guard<std::mutex> lock(_mutex);
        _graph->add(object); 
    }
}

void flap::GraphManager::addObject(std::vector<std::shared_ptr<dibiff::graph::AudioObject>> objects) { 
    /// Mutex Locked
    {
        // std::lock_guard<std::mutex> lock(_mutex);
        for (auto object : objects) { 
            _graph->add(object); 
        } 
    }
}

void flap::GraphManager::addObject(std::shared_ptr<dibiff::graph::AudioCompositeObject> object) { 
    /// Mutex Locked
    {
        // std::lock_guard<std::mutex> lock(_mutex);
        _graph->add(object);
    }
}


void flap::GraphManager::_threadFunction() {
    while (isRunning()) {
        /// Mutex Locked
        {
            std::lock_guard<std::mutex> lock(_mutex);
            _graph->tick();
            /// Wait for the signal from the audio manager
            {
                std::unique_lock<std::mutex> ulock(*_graphMutex);
                _graphSignal->wait(ulock);
            }
        }
        /// Mutex Unlocked
    }
}