/// GraphManager.cpp

#include "GraphManager.h"

bool flap::GraphManager::initialize() {
    _mutex = std::make_unique<std::mutex>();
    _graph = std::make_unique<dibiff::graph::AudioGraph>();
    return true;
}

void flap::GraphManager::cleanup() {
}

void flap::GraphManager::addObject(dibiff::graph::AudioObject* object) { 
    /// Mutex Locked
    {
        // std::lock_guard<std::mutex> lock(_mutex);
        _graph->add(object);
    }
}

void flap::GraphManager::addObject(std::vector<dibiff::graph::AudioObject*> objects) { 
    /// Mutex Locked
    {
        // std::lock_guard<std::mutex> lock(_mutex);
        for (auto& object : objects) { 
            _graph->add(object); 
        } 
    }
}

void flap::GraphManager::addObject(dibiff::graph::AudioCompositeObject* object) { 
    /// Mutex Locked
    {
        // std::lock_guard<std::mutex> lock(_mutex);
        _graph->add(object);
    }
}

void flap::GraphManager::removeObject(dibiff::graph::AudioObject* object) { 
    /// Mutex Locked
    {
        // std::lock_guard<std::mutex> lock(*_mutex);
        _graph->remove(object); 
    }
}

void flap::GraphManager::removeObject(std::vector<dibiff::graph::AudioObject*> objects) { 
    /// Mutex Locked
    {
        // std::lock_guard<std::mutex> lock(*_mutex);
        for (auto& object : objects) { 
            _graph->remove(object); 
        } 
    }
}

void flap::GraphManager::removeObject(dibiff::graph::AudioCompositeObject* object) { 
    /// Mutex Locked
    {
        // std::lock_guard<std::mutex> lock(*_mutex);
        _graph->remove(object);
    }
}

void flap::GraphManager::_threadFunction() {
    while (isRunning()) {
        /// wait for the input signal from the audio manager
        for (int i = 0; i < _inputGraphSignals.size(); i++) {
            std::unique_lock<std::mutex> ulock(*_inputGraphMutexs[i]);
            _inputGraphSignals[i]->wait(ulock);
        }
        /// Reset the signals
        for (auto& signal : _inputGraphSignals) {
            signal->notify_all();
        }
        /// Mutex Locked
        {   
            /// Process a tick of the audio graph
            std::lock_guard<std::mutex> lock(*_mutex);
            _graph->tick();
        }
        /// Mutex Unlocked
        /// Wait for the output signal from the audio manager
        for (int i = 0; i < _outputGraphSignals.size(); i++) {
            std::unique_lock<std::mutex> ulock(*_outputGraphMutexs[i]);
            _outputGraphSignals[i]->wait(ulock);
        }
        /// Reset the signals
        for (auto& signal : _outputGraphSignals) {
            signal->notify_all();
        }
    }
}