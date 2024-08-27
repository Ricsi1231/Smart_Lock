#pragma once

#include <iostream>
#include "esp_log.h"
#include "driver/uart.h"
#include "esp_err.h"
#include "driver/gpio.h"

#ifdef CONFIG_IDF_TARGET_ESP32
    #define ESP_DEFAULT_UART_TX_PIN (GPIO_NUM_5)
    #define ESP_DEFAULT_UART_RX_PIN (GPIO_NUM_6)
    #define ESP_DEFAULT_UART_CTS_PIN (GPIO_NUM_10)
    #define ESP_DEFAULT_UART_RTS_PIN (GPIO_NUM_11)
    #define ESP_DEFAULT_UART_PORT (UART_NUM_1)
#elif CONFIG_IDF_TARGET_ESP32S2
    #define ESP_DEFAULT_UART_TX_PIN (GPIO_NUM_5)
    #define ESP_DEFAULT_UART_RX_PIN (GPIO_NUM_6)
    #define ESP_DEFAULT_UART_CTS_PIN (GPIO_NUM_10)
    #define ESP_DEFAULT_UART_RTS_PIN (GPIO_NUM_11)
    #define ESP_DEFAULT_UART_PORT (UART_NUM_1)
#elif CONFIG_IDF_TARGET_ESP32C6
    #define ESP_DEFAULT_UART_TX_PIN (GPIO_NUM_5)
    #define ESP_DEFAULT_UART_RX_PIN (GPIO_NUM_6)
    #define ESP_DEFAULT_UART_CTS_PIN (GPIO_NUM_10)
    #define ESP_DEFAULT_UART_RTS_PIN (GPIO_NUM_11)
    #define ESP_DEFAULT_UART_PORT (UART_NUM_1)

#endif

namespace UartDriver {
    class UartDriver {
        public:
            explicit UartDriver();
            virtual ~UartDriver();

            virtual esp_err_t init(uart_port_t uartPort, uart_config_t uartConfig, uint8_t rxPin, uint8_t txPin, uint8_t ctsPin, uint8_t rtsPin,
            uint8_t rxBufferSize, uint8_t txBufferSize, uint8_t queueSize, QueueHandle_t *uartQueue, uint8_t flagAllocateInterrupt);
            virtual esp_err_t sendData(uint8_t *data);

        private:
    };
}