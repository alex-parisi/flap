/// AudioIn.h

#pragma once

#include "../Object.h"
#include "miniaudio.h"
#include "../../widgets/Connector.h"

#include <memory>
#include <optional>
#include <string>

namespace flap {
    class AudioIn : public Object, public std::enable_shared_from_this<AudioIn> {
    public:
        inline AudioIn(ma_device_info device, int channels, std::optional<std::string> name = std::nullopt) : Object(), _device(device), _channels(channels), _name(name) {}
        void initialize() override;
        void render() override;
        void preDisconnect() override;
        void postDisconnect() override;
    private:
        ma_device_info _device;
        int _channels;
        Connector _output;
        Connector _outputL;
        Connector _outputR;
        std::optional<std::string> _name;
    };
}