#include "Lock_driver.hpp"

namespace LockSystem {
    Lock::Lock() {

    }

    Lock::~Lock() {
        
    }

    esp_err_t Lock::init(uint8_t rxPin, uint8_t txPin) {
        esp_err_t espCheck = ESP_OK;
        
        uint8_t rxBufferSize = 256;
        uint8_t txBufferSize = 256;

        uint8_t queueSize = 0;
        QueueHandle_t *uartQueue = NULL;

        uint8_t flagAllocateInterrupt = 0;

        if(rxPin == 0 || txPin == 0) {
            espCheck = ESP_ERR_INVALID_ARG;
            return espCheck;
        }

        uart_config_t uartConfig = {
            .baud_rate = 115200,
            .data_bits = UART_DATA_8_BITS,
            .parity = UART_PARITY_DISABLE,
            .stop_bits = UART_STOP_BITS_1,
            .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        };

        espCheck = uart.init(UART_NUM_1, uartConfig, rxPin, txPin, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, rxBufferSize, txBufferSize, queueSize, uartQueue, flagAllocateInterrupt);
    
        return espCheck;
    }

    esp_err_t Lock::setKey() {
        esp_err_t espCheck = ESP_OK;
        
        uint8_t payload[] = {
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x55, 0xAA, 0x03, 0x55, 0xAA, 0x00, 0x00, 
            0x00, 0x01, 0x55, 0xAA, 0x03, 0x00, 0x03, 0x04, 0x00, 0x19, 0x30, 0x00, 0x00, 0x15,
            0x01, 0xAA, 0xAA, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x48,
            0x48, 0x48, 0x48, 0x48, 0x48, 0x48, 0x48, 0xA0
        };

        uint8_t dataLen = sizeof(payload);

        espCheck = uart.sendData(payload, dataLen);
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

        espCheck = uart.sendData(payload, dataLen);
        return espCheck;
    }

    esp_err_t Lock::turnOffAutomaticLatch() {
        esp_err_t espCheck = ESP_OK;

        uint8_t dataLen = 0;

        // uint8_t payload1[] = {
        //     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x55, 0xAA, 0x03, 0x55, 0xAA, 0x00, 0x00, 0x00, 0x01, //* Wakeup
        //     0x55, 0xAA, 0x03, 0x00, 0x23, 0x00, 0x00, 0x00, 0x07, 0x08, 0x01, 0x01, 0x01
        // };

        uint8_t wakeup[] = {
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x55, 0xAA, 0x03, 0x55, 0xAA, 0x00, 0x00, 0x00, 0x01 //* Wakeup
        };

        uint8_t disableLatch[] = {
            0x55, 0xAA, 0x03, 0x00, 0x23, 0x00, 0x00
        };
        
        dataLen = sizeof(wakeup);

        espCheck = uart.sendData(wakeup, dataLen);

        dataLen = sizeof(disableLatch);

        vTaskDelay(50 / portTICK_PERIOD_MS);

        espCheck = uart.sendData(disableLatch, dataLen);
        
        return espCheck;
    }
}