/// GraphManager.cpp

#include "GraphManager.h"

bool GraphManager::initialize() {
    _graph = std::make_shared<dibiff::graph::AudioGraph>();
    return true;
}

void GraphManager::cleanup() {
}

void GraphManager::_threadFunction() {
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