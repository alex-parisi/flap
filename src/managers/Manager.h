/// Manager.h

#pragma once

#include <thread>
#include <mutex>
#include <atomic>
#include <memory>

#include "../MainApplicationSettings.h"

namespace flap {
    class Manager {
        public:
            Manager(std::shared_ptr<MainApplicationSettings> settings) : _settings(settings), _running(false) {}
            virtual bool initialize() = 0;
            virtual void cleanup() = 0;
            void run() {
                _running = true;
                _thread = std::thread(&Manager::_threadFunction, this);
            }
            void stop() {
                _running = false;
                if (_thread.joinable()) _thread.join();
            }
            bool isRunning() { return _running; }
            void setSettings(std::shared_ptr<MainApplicationSettings> settings) { 
                std::lock_guard<std::mutex> lock(_mutex);
                _settings = settings;
            }
        protected:
            std::shared_ptr<flap::MainApplicationSettings> _settings;
            std::atomic<bool> _running;
            std::thread _thread;
            mutable std::mutex _mutex;
            virtual void _threadFunction() = 0;
    };
}