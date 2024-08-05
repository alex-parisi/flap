/// Object.h

#pragma once

#include <vector>

#include "dibiff/dibiff"

namespace flap {
    class Object : public std::enable_shared_from_this<Object> {
        public:
            virtual void initialize() = 0;
            virtual void render() = 0;
            std::vector<std::shared_ptr<dibiff::graph::AudioObject>> getAudioObjects() { return _audioObjects; }
        protected:
            std::vector<std::shared_ptr<dibiff::graph::AudioObject>> _audioObjects;
    };
}