/// KeyboardService.h

#pragma once

class ConnectionService {
    public:
        /// Singleton pattern
        inline static ConnectionService& getInstance() {
            static ConnectionService instance; // Guaranteed to be destroyed and instantiated on first use
            return instance;
        }
        /// Delete copy constructor and assignment operator to prevent copying
        ConnectionService(const ConnectionService&) = delete;
        ConnectionService& operator=(const ConnectionService&) = delete;
    private:
        /// Singleton pattern
        ConnectionService() {}
        ~ConnectionService() {}
};