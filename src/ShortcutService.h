/// ShortcutService.h

#pragma once

#include "imgui.h"

#include <functional>
#include <vector>
#include "KeyboardService.h"

namespace flap {
    struct KeyboardShortcut {
        std::vector<ImGuiKey> keys = {};
        std::function<void()> callback = [](){};
    };
    class ShortcutService {
        public:
            /// Singleton pattern
            inline static ShortcutService& getInstance() {
                static ShortcutService instance;
                return instance;
            }
            /// Delete copy constructor and assignment operator to prevent copying
            ShortcutService(const ShortcutService&) = delete;
            ShortcutService& operator=(const ShortcutService&) = delete;
            inline void update() {
                for (auto& shortcut : shortcuts) {
                    int keys = 0;
                    for (auto key : shortcut.keys) {
                        keys += KeyboardService::getInstance().isKeyDown(key);
                    }
                    signed int kSize = shortcut.keys.size();
                    if (keys > kSize) {
                        shortcut.callback();
                    }
                }
            }
            std::vector<KeyboardShortcut> shortcuts;
        private:
            /// Singleton pattern
            inline ShortcutService() {}
            inline ~ShortcutService() {}
    };
}