/// Manager.h

#pragma once

#include <thread>
#include <mutex>

class Manager {
    public:
        Manager(int blockSize = 512) : _blockSize(blockSize), _running(false) {}
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
        void setBlockSize(int blockSize) { _blockSize = blockSize; }
    protected:
        int _blockSize;
        std::atomic<bool> _running;
        std::thread _thread;
        mutable std::mutex _mutex;
        virtual void _threadFunction() = 0;
};