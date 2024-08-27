#pragma once

#include "uart_driver.hpp"

namespace Lock {
    class Lock: public UartDriver::UartDriver {
        public:
            explicit Lock();
            virtual ~Lock();

            esp_err_t initalize(uart_port_t uartPort, uint8_t rxPin, uint8_t txPin);
            esp_err_t open();
    };
}