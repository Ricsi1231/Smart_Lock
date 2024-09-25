#pragma once

#include <cstdint>
#include <iostream>
#include "uart_driver.hpp"

namespace LockSystem {
    class Lock {
        public:
        Lock();
        virtual ~Lock();

        esp_err_t init(uint8_t rxPin, uint8_t txPin);
        esp_err_t setKey();
        esp_err_t open();
        // bool getStatus();

        private:
        UartDriver::UartDriver uart;
    };
}