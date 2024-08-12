/// GraphManager.cpp

#include "GraphManager.h"

bool flap::GraphManager::initialize() {
    _mutex = std::make_shared<std::recursive_mutex>();
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

void flap::GraphManager::removeObject(std::shared_ptr<dibiff::graph::AudioObject> object) { 
    /// Mutex Locked
    {
        // std::lock_guard<std::mutex> lock(*_mutex);
        _graph->remove(object); 
    }
}

void flap::GraphManager::removeObject(std::vector<std::shared_ptr<dibiff::graph::AudioObject>> objects) { 
    /// Mutex Locked
    {
        // std::lock_guard<std::mutex> lock(*_mutex);
        for (auto object : objects) { 
            _graph->remove(object); 
        } 
    }
}

void flap::GraphManager::removeObject(std::shared_ptr<dibiff::graph::AudioCompositeObject> object) { 
    /// Mutex Locked
    {
        // std::lock_guard<std::mutex> lock(*_mutex);
        _graph->remove(object);
    }
}

void flap::GraphManager::_threadFunction() {
    while (isRunning()) {
        /// Mutex Locked
        {
            std::lock_guard<std::recursive_mutex> lock(*_mutex);
            _graph->tick();
            /// Wait for the signal from the audio manager
            for (int i = 0; i < _graphSignals.size(); i++) {
                std::unique_lock<std::mutex> ulock(*_graphMutexs[i]);
                _graphSignals[i]->wait(ulock);
            }
            /// Reset the signals
            for (auto& signal : _graphSignals) {
                signal->notify_all();
            }
        }
        /// Mutex Unlocked
    }
}