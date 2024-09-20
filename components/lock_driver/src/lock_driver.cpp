#include "lock_driver.hpp"

namespace LockSystem {
    Lock::Lock() {

    }

    Lock::~Lock() {

    }

    esp_err_t Lock::initalize(uart_port_t uartPort, uint8_t rxPin, uint8_t txPin) {
        esp_err_t espCheck = ESP_OK;
        
        uart_config_t uart_config = {
            .baud_rate = 9600,
            .data_bits = UART_DATA_8_BITS,
            .parity = UART_PARITY_DISABLE,
            .stop_bits = UART_STOP_BITS_1,
            .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        };

        espCheck = init(uartPort, uart_config, rxPin, txPin, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, 256, 256, 0, NULL, 0);
        return espCheck;
    }

    esp_err_t Lock::open() {
        esp_err_t espCheck = ESP_OK;

        uint8_t payload[] = {
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x55, 0xAA, 0x03, 0x55, 0xAA, 0x00, 0x00, 0x00, 0x01, //* Wakeup
            0x55, 0xAA, 0x03, 0x00, 0x04, 0x04, 0x00, 0x0F, 0x31, 0x00, 0x00, 0x0B, 0x01, 0xAA, 0xAA, 0x48, //* Command
            0x48, 0x48, 0x48, 0x48, 0x48, 0x48, 0x48, 0xFB
        };
        uint8_t dataLen = sizeof(payload);

        espCheck = sendData(payload, dataLen);
        return espCheck;
    }
}