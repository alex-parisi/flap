/// Object.h

#pragma once

#include <vector>
#include <iostream>

#include "imgui.h"
#include "dibiff/dibiff"
#include "../utilities/BoolProxy.h"

namespace flap {
    class Object : public std::enable_shared_from_this<Object> {
        public:
            Object();
            virtual ~Object() = default;
            virtual void initialize() = 0;
            virtual void render() = 0;
            std::vector<std::shared_ptr<dibiff::graph::AudioObject>> getAudioObjects() { return _audioObjects; }
            bool shouldDelete = false;
        protected:
            std::vector<std::shared_ptr<dibiff::graph::AudioObject>> _audioObjects;
            BoolProxy _isOpen;
    };
}