#pragma once

#include <iostream>
#include "esp_log.h"
#include "driver/uart.h"
#include "esp_err.h"
#include "driver/gpio.h"

#define ESP_DEFAULT_UART_TX_PIN (GPIO_NUM_5)
#define ESP_DEFAULT_UART_RX_PIN (GPIO_NUM_4)
#define ESP_DEFAULT_UART_CTS_PIN (GPIO_NUM_10)
#define ESP_DEFAULT_UART_RTS_PIN (GPIO_NUM_11)
#define ESP_DEFAULT_UART_PORT (UART_NUM_1)

namespace UartDriver {
    class UartDriver {
        public:
            explicit UartDriver();
            virtual ~UartDriver();

            virtual esp_err_t init(uart_port_t uartPort, uart_config_t uartConfig, uint8_t rxPin, uint8_t txPin, uint8_t ctsPin, uint8_t rtsPin,
            int rxBufferSize, int txBufferSize, uint8_t queueSize, QueueHandle_t *uartQueue, uint8_t flagAllocateInterrupt);
            virtual esp_err_t sendData(uint8_t *data, size_t length);

        protected:
            const char *TAG = "ESP32_LOCK_UART";
    };
}