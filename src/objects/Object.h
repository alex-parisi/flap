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
            inline std::vector<dibiff::graph::AudioObject*> getAudioObjects() { 
                std::vector<dibiff::graph::AudioObject*> audioObjects;
                for (auto& audioObject : _audioObjects) {
                    audioObjects.push_back(audioObject.get());
                }
                return audioObjects;
            }
            bool shouldDelete = false;
            virtual void preDisconnect() {};
            virtual void postDisconnect() {};
        protected:
            std::vector<std::unique_ptr<dibiff::graph::AudioObject>> _audioObjects;
            BoolProxy _isOpen;
    };
}