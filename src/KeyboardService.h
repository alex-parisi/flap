/// KeyboardService.h

#pragma once

#include "imgui.h"
#include <map>

namespace flap {
    class KeyboardService {
        public:
            /// Singleton pattern
            inline static KeyboardService& getInstance() {
                static KeyboardService instance;
                return instance;
            }
            /// Delete copy constructor and assignment operator to prevent copying
            KeyboardService(const KeyboardService&) = delete;
            KeyboardService& operator=(const KeyboardService&) = delete;
            inline void update() {
                for (int i = 0; i < ImGuiKey_COUNT; i++) {
                    if (ImGui::IsKeyDown((ImGuiKey)i)) {
                        if (keyMap[(ImGuiKey)i] <= 0) {
                            keyMap[(ImGuiKey)i] = 2;
                        } else {
                            keyMap[(ImGuiKey)i] = 1;
                        }
                    } else if (ImGui::IsKeyReleased((ImGuiKey)i, false)) {
                        keyMap[(ImGuiKey)i] = -1;
                    } else {
                        keyMap[(ImGuiKey)i] = 0;
                    }
                }
            }
            inline int isKeyDown(ImGuiKey key) {
                if (keyMap.find(key) == keyMap.end()) {
                    return 0;
                }
                return keyMap[key];
            }
        private:
            /// Singleton pattern
            inline KeyboardService() {}
            inline ~KeyboardService() {}
            std::map<ImGuiKey, int> keyMap;
    };
}