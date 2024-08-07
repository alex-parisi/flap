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
             * @param settings The settings for the main application.
             */
            Manager(std::shared_ptr<MainApplicationSettings> settings) : _settings(settings), _running(false) {}
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
            void run() {
                _running = true;
                _thread = std::thread(&Manager::_threadFunction, this);
            }
            /**
             * @brief Stops the Manager.
             */
            void stop() {
                _running = false;
                if (_thread.joinable()) _thread.join();
            }
            /**
             * @brief Checks if the Manager is running.
             */
            bool isRunning() { return _running; }
            /**
             * @brief Sets the settings for the Manager.
             */
            void setSettings(std::shared_ptr<MainApplicationSettings> settings) { 
                std::lock_guard<std::mutex> lock(*_mutex);
                _settings = settings;
            }
            /**
             * @brief Gets the mutex for the Manager.
             */
            std::shared_ptr<std::mutex> getMutex() { return _mutex; }
        protected:
            /**
             * @brief The settings for the main application.
             */
            std::shared_ptr<flap::MainApplicationSettings> _settings;
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