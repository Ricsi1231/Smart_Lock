#include "uart_driver.hpp"

namespace UartDriver {
    UartDriver::UartDriver() {

    }

    UartDriver::~UartDriver() {

    }

    esp_err_t UartDriver::init(uart_port_t uartPort, uart_config_t uartConfig, uint8_t rxPin, uint8_t txPin, uint8_t ctsPin, uint8_t rtsPin,
        int rxBufferSize, int txBufferSize, uint8_t queueSize, QueueHandle_t *uartQueue, uint8_t flagAllocateInterrupt) {
        esp_err_t espCheck = ESP_OK;

    //    if(uartPort != ESP_DEFAULT_UART_PORT || rxPin != ESP_DEFAULT_UART_RX_PIN || txPin != ESP_DEFAULT_UART_TX_PIN) {
    //         espCheck = ESP_ERR_INVALID_ARG;
    //         return espCheck;
    //     }

    //     if((queueSize == 0 && uartQueue != NULL) || (queueSize != 0 && uartQueue == NULL)) {
    //         espCheck = ESP_ERR_INVALID_ARG;
    //         return espCheck;
    //     }

    //     if(rxBufferSize == 0 || txBufferSize == 0) {
    //         espCheck = ESP_ERR_INVALID_ARG;
    //         return espCheck;
    //     }

    //     if(uartConfig.baud_rate == 0 || uartConfig.data_bits == 0 || uartConfig.parity == 0 || uartConfig.stop_bits == 0) {
    //         espCheck = ESP_ERR_INVALID_ARG;
    //         return espCheck;
    //     }

        espCheck = uart_param_config(ESP_DEFAULT_UART_PORT, &uartConfig);
        
        if(ctsPin == UART_PIN_NO_CHANGE && rtsPin == UART_PIN_NO_CHANGE) {
            espCheck = uart_set_pin(ESP_DEFAULT_UART_PORT, ESP_DEFAULT_UART_TX_PIN, ESP_DEFAULT_UART_RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
        }
        else {
            espCheck = uart_set_pin(ESP_DEFAULT_UART_PORT, ESP_DEFAULT_UART_TX_PIN, ESP_DEFAULT_UART_RX_PIN, ESP_DEFAULT_UART_CTS_PIN, ESP_DEFAULT_UART_RTS_PIN);
        }

        espCheck = uart_driver_install(ESP_DEFAULT_UART_PORT, 256, 0, queueSize, uartQueue, flagAllocateInterrupt);
   
        ESP_LOGI(TAG, "\nUART init %x", espCheck);

        return espCheck;
    }

    esp_err_t UartDriver::sendData(uint8_t *data, size_t length) {
        esp_err_t espCheck = ESP_OK;
        int uartWriteBytes = 0;

        if(uart_is_driver_installed(ESP_DEFAULT_UART_PORT) == false) {
            espCheck = ESP_FAIL;
            return espCheck;
        }

        uartWriteBytes = uart_write_bytes(ESP_DEFAULT_UART_PORT, data, length);

        if(uartWriteBytes < 0) {
            espCheck = ESP_FAIL;
            return espCheck;
        }

        ESP_LOGI(TAG, "Data: %d\n", uartWriteBytes);
        return espCheck;
    }
}