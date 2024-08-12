/// ObjectManager.h

#pragma once

#include <vector>
#include "../objects/Object.h"

namespace flap {
    class ObjectManager {
        public:
            /// Singleton pattern
            static ObjectManager& getInstance() {
                static ObjectManager instance;
                return instance;
            }
            ObjectManager(const ObjectManager&) = delete;
            ObjectManager& operator=(const ObjectManager&) = delete; 

            void cleanup() {
                objects.clear();
            }
            std::vector<std::shared_ptr<Object>> objects;
            std::shared_ptr<std::recursive_mutex> mutex;
        private:
            /// Singleton pattern
            ObjectManager() {}
            ~ObjectManager() {}
    };
}