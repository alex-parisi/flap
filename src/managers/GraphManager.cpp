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
            /// wait for the input signal from the audio manager
            for (int i = 0; i < _inputGraphSignals.size(); i++) {
                std::unique_lock<std::mutex> ulock(*_inputGraphMutexs[i]);
                _inputGraphSignals[i]->wait(ulock);
            }
            /// Reset the signals
            for (auto& signal : _inputGraphSignals) {
                signal->notify_all();
            }
            /// Process a tick of the audio graph
            std::lock_guard<std::recursive_mutex> lock(*_mutex);
            _graph->tick();
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
        /// Mutex Unlocked
    }
}