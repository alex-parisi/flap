/// ObjectManager.h

#pragma once

#include <vector>
#include "../objects/Object.h"

namespace flap {
    class ObjectManager {
        public:
            /// Singleton pattern
            inline static ObjectManager& getInstance() {
                static ObjectManager instance;
                return instance;
            }
            ObjectManager(const ObjectManager&) = delete;
            ObjectManager& operator=(const ObjectManager&) = delete; 

            inline void cleanup() {
                objects.clear();
            }
            std::vector<std::shared_ptr<Object>> objects;
            std::shared_ptr<std::recursive_mutex> mutex;
            /// TODO: Use a better way than counters to manage object IDs
            int gainCounter = 0;
            int envelopeCounter = 0;
            int chorusCounter = 0;
            int flangerCounter = 0;
            int phaserCounter = 0;
            int sineCounter = 0;
            int probeCounter = 0;
            int keyboardSimulatorCounter = 0;
            int flapSynthCounter = 0;
            int echoCancellerCounter = 0;
        private:
            /// Singleton pattern
            inline ObjectManager() {}
            inline ~ObjectManager() {}
    };
}