/// GraphManager.h

#pragma once

#include "Manager.h"
#include "dibiff/dibiff"

#include <condition_variable>
#include <mutex>

namespace flap {
    /**
     * @brief GraphManager is a class that manages audio objects and the audio graph.
     */
    class GraphManager : public Manager {
        public:
            /// Singleton pattern
            static GraphManager& getInstance() {
                static GraphManager instance;
                return instance;
            }
            /// Delete copy constructor and assignment operator to prevent copying
            GraphManager(const GraphManager&) = delete;
            GraphManager& operator=(const GraphManager&) = delete; 
            /**
             * @brief Initializes the GraphManager.
             */
            bool initialize() override;
            /**
             * @brief Cleans up the GraphManager.
             */
            void cleanup() override;
            /**
             * @brief Adds an object to the graph.
             * @param object The object to add.
             */
            void addObject(std::shared_ptr<dibiff::graph::AudioObject> object);
            /**
             * @brief Adds a vector of objects to the graph.
             * @param objects The objects to add.
             */
            void addObject(std::vector<std::shared_ptr<dibiff::graph::AudioObject>> objects);
            /**
             * @brief Adds a composite object to the graph.
             * @param object The object to add.
             */
            void addObject(std::shared_ptr<dibiff::graph::AudioCompositeObject> object);
            /**
             * @brief Removes an object from the graph.
             */
            void removeObject(std::shared_ptr<dibiff::graph::AudioObject> object);
            /**
             * @brief Removes a vector of objects from the graph.
             */
            void removeObject(std::vector<std::shared_ptr<dibiff::graph::AudioObject>> objects);
            /**
             * @brief Removes a composite object from the graph.
             */
            void removeObject(std::shared_ptr<dibiff::graph::AudioCompositeObject> object);
            /**
             * @brief Adds a graph signal for the GraphManager to wait on
             */
            void addGraphSignal(std::condition_variable * signal) { _graphSignals.push_back(signal); }
            /**
             * @brief Adds a graph mutex for the GraphManager to lock
             */
            void addGraphMutex(std::mutex * mutex) { _graphMutexs.push_back(mutex); }
            /// Override the mutex
            std::shared_ptr<std::recursive_mutex> getMutex() { return _mutex; }
        private:
            /// Override the mutex
            std::shared_ptr<std::recursive_mutex> _mutex;
            /// Singleton pattern
            GraphManager() {}
            ~GraphManager() {}
            /// TODO: Use smart pointers here
            /**
             * @brief A vector of condition variables for the GraphManager to wait on
             */
            std::vector<std::condition_variable *> _graphSignals;
            /**
             * @brief A vector of mutexes for the GraphManager to lock
             */
            std::vector<std::mutex *> _graphMutexs;
            /**
             * @brief The thread function for the GraphManager.
             */
            void _threadFunction() override;
            /**
             * @brief The audio graph.
             */
            std::shared_ptr<dibiff::graph::AudioGraph> _graph;
    };
}