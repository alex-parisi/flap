/// GraphManager.h

#pragma once

#include "Manager.h"
#include "dibiff/dibiff"

#include <condition_variable>
#include <mutex>

namespace flap {
    class GraphManager : public Manager {
        public: 
            GraphManager(std::shared_ptr<MainApplicationSettings> settings) : Manager(settings) {}
            bool initialize() override;
            void cleanup() override;
            void addObject(std::shared_ptr<dibiff::graph::AudioObject> object);
            void addObject(std::vector<std::shared_ptr<dibiff::graph::AudioObject>> objects);
            void addObject(std::shared_ptr<dibiff::graph::AudioCompositeObject> object);
            void setGraphSignal(std::condition_variable * signal) { _graphSignal = signal; }
            void setGraphMutex(std::mutex * mutex) { _graphMutex = mutex; }
        private:
            /// TODO: Use smart pointers here
            std::condition_variable * _graphSignal;
            std::mutex * _graphMutex;
            void _threadFunction() override;
            std::shared_ptr<dibiff::graph::AudioGraph> _graph;
    };
}