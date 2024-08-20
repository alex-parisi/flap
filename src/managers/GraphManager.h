/// GraphManager.h

#pragma once

#include "Manager.h"
#include "dibiff/dibiff"

#include <condition_variable>
#include <mutex>
#include <chrono>

namespace flap {
    /**
     * @brief GraphManager is a class that manages audio objects and the audio graph.
     */
    class GraphManager : public Manager {
        public:
            /// Singleton pattern
            inline static GraphManager& getInstance() {
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
            void addObject(dibiff::graph::AudioObject* object);
            /**
             * @brief Adds a vector of objects to the graph.
             * @param objects The objects to add.
             */
            void addObject(std::vector<dibiff::graph::AudioObject*> objects);
            /**
             * @brief Adds a composite object to the graph.
             * @param object The object to add.
             */
            void addObject(dibiff::graph::AudioCompositeObject* object);
            /**
             * @brief Removes an object from the graph.
             */
            void removeObject(dibiff::graph::AudioObject* object);
            /**
             * @brief Removes a vector of objects from the graph.
             */
            void removeObject(std::vector<dibiff::graph::AudioObject*> objects);
            /**
             * @brief Removes a composite object from the graph.
             */
            void removeObject(dibiff::graph::AudioCompositeObject* object);
            /**
             * @brief Adds a graph signal for the GraphManager to wait on
             */
            inline void addOutputGraphSignal(std::condition_variable * signal) { _outputGraphSignals.push_back(signal); }
            inline void addInputGraphSignal(std::condition_variable * signal) { _inputGraphSignals.push_back(signal); }
            /**
             * @brief Adds a graph mutex for the GraphManager to lock
             */
            inline void addOutputGraphMutex(std::mutex * mutex) { _outputGraphMutexs.push_back(mutex); }
            inline void addInputGraphMutex(std::mutex * mutex) { _inputGraphMutexs.push_back(mutex); }
            inline std::mutex& getMutex() { return *_mutex; }
            inline void removeOutputGraphSignal(std::condition_variable * signal) { 
                _outputGraphSignals.erase(std::remove_if(_outputGraphSignals.begin(), _outputGraphSignals.end(), [&](std::condition_variable * s) { return s == signal; }), _outputGraphSignals.end());
            }
            inline void removeInputGraphSignal(std::condition_variable * signal) { 
                _inputGraphSignals.erase(std::remove_if(_inputGraphSignals.begin(), _inputGraphSignals.end(), [&](std::condition_variable * s) { return s == signal; }), _inputGraphSignals.end());
            }
            inline void removeOutputGraphMutex(std::mutex * mutex) { 
                _outputGraphMutexs.erase(std::remove_if(_outputGraphMutexs.begin(), _outputGraphMutexs.end(), [&](std::mutex * m) { return m == mutex; }), _outputGraphMutexs.end());
            }
            inline void removeInputGraphMutex(std::mutex * mutex) { 
                _inputGraphMutexs.erase(std::remove_if(_inputGraphMutexs.begin(), _inputGraphMutexs.end(), [&](std::mutex * m) { return m == mutex; }), _inputGraphMutexs.end());
            }
        private:
            /// Override the mutex
            std::unique_ptr<std::mutex> _mutex;
            /// Singleton pattern
            inline GraphManager() {}
            inline ~GraphManager() {}
            /// TODO: Use smart pointers here
            /**
             * @brief A vector of condition variables for the GraphManager to wait on
             */
            std::vector<std::condition_variable *> _outputGraphSignals;
            std::vector<std::condition_variable *> _inputGraphSignals;
            /**
             * @brief A vector of mutexes for the GraphManager to lock
             */
            std::vector<std::mutex *> _outputGraphMutexs;
            std::vector<std::mutex *> _inputGraphMutexs;
            /**
             * @brief The thread function for the GraphManager.
             */
            void _threadFunction() override;
            /**
             * @brief The audio graph.
             */
            std::unique_ptr<dibiff::graph::AudioGraph> _graph;
    };
}