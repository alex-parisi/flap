/// GraphManager.cpp

#include "GraphManager.h"

bool GraphManager::initialize() {
    return true;
}

void GraphManager::cleanup() {
}

void GraphManager::_threadFunction() {
    while (isRunning()) {
        /// Mutex Locked
        {
            std::lock_guard<std::mutex> lock(_mutex);
            /// Do graph stuff
        }
        /// Mutex Unlocked
    }
}