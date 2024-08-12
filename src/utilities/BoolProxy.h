/// BoolProxy.h

#pragma once

#include <iostream>
#include <functional>

class BoolProxy {
    public:
        using Callback = std::function<void()>;
        inline BoolProxy() : value_(true) {}
        inline BoolProxy(Callback onFalse) : value_(true), onFalse_(onFalse) {}
        inline operator bool*() {
            return &value_;
        }
        inline void check() {
            if (!value_ && onFalse_) {
                onFalse_();
                // Reset value to true to prevent continuous calling
                value_ = true;
            }
        }
    private:
        bool value_;
        Callback onFalse_;
};