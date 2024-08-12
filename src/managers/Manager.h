/// Manager.h

#pragma once

#include <thread>
#include <mutex>
#include <atomic>
#include <memory>

#include "../MainApplicationSettings.h"

namespace flap {
    /**
     * @brief Manager is a class that manages a specific part of the application.
     */
    class Manager {
        public:
            /**
             * @brief Manager constructor.
             */
            inline Manager() : _running(false) {}
            /**
             * @brief Initializes the Manager.
             */
            virtual bool initialize() = 0;
            /**
             * @brief Cleans up the Manager.
             */
            virtual void cleanup() = 0;
            /**
             * @brief Runs the Manager.
             */
            inline void run() {
                _running = true;
                _thread = std::thread(&Manager::_threadFunction, this);
            }
            /**
             * @brief Stops the Manager.
             */
            inline void stop() {
                _running = false;
                if (_thread.joinable()) _thread.join();
            }
            /**
             * @brief Checks if the Manager is running.
             */
            inline bool isRunning() { return _running; }
            /**
             * @brief Gets the mutex for the Manager.
             */
            inline std::shared_ptr<std::mutex> getMutex() { return _mutex; }
        protected:
            /**
             * @brief A flag to check if the Manager is running.
             */
            std::atomic<bool> _running;
            /**
             * @brief The thread for the Manager.
             */
            std::thread _thread;
            /**
             * @brief The mutex for the Manager.
             */
            std::shared_ptr<std::mutex> _mutex;
            /**
             * @brief The thread function for the Manager.
             */
            virtual void _threadFunction() = 0;
    };
}