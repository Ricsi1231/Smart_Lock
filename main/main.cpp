#include <stdio.h>
#include "lock_driver.hpp"
#include "zigbee.hpp"
#include "esp_zigbee_core.h"
#include "esp_zigbee_secur.h"

#define UART_PORT (UART_NUM_1)
#define UART_RX_PIN (GPIO_NUM_4)
#define UART_TX_PIN (GPIO_NUM_5)

LockSystem::Lock Lock;

void esp_zb_app_signal_handler(esp_zb_app_signal_t *signal_s);

extern "C" void app_main(void) {
    const char *TAG = "ESP32_LOCK_UART";

    Lock.initalize(UART_PORT, UART_RX_PIN, UART_TX_PIN);

    ZigbeeComponent::Zigbee::init();

    uint8_t key[] = {0x83, 0x05, 0xD3, 0x90, 0x99, 0xF3, 0x9D, 0x71, 0xF1, 0x78, 0x60, 0xD4, 0x90, 0x75, 0xCF, 0x99};
    esp_err_t espStatus = esp_zb_secur_network_key_set(key);
    ESP_LOGI(TAG, "Network key: %d", espStatus);    

    //Lock.open();
    xTaskCreate(ZigbeeComponent::Zigbee::rtosTask, "Zigbee_main", 4096, NULL, 5, NULL);
}

void esp_zb_app_signal_handler(esp_zb_app_signal_t *signal_s) {
        uint32_t *p_signal = signal_s->p_app_signal;
        esp_err_t err_status = signal_s->esp_err_status;
        esp_zb_app_signal_type_t sig_type = static_cast<esp_zb_app_signal_type_t>(*p_signal);

        const char *TAG = "ESP32_LOCK_UART";
        
        //! sig_type = ESP_ZB_BDB_SIGNAL_STEERING;
        ESP_LOGI(TAG, "Signal type: %x", sig_type);

        switch(sig_type) {
            case ESP_ZB_ZDO_SIGNAL_SKIP_STARTUP:
                esp_zb_ieee_addr_t extended_pan_id;
                extended_pan_id[0] = 0x74;
                extended_pan_id[1] = 0x4D;
                extended_pan_id[2] = 0xBD;
                extended_pan_id[3] = 0xFF;
                extended_pan_id[4] = 0xFE;
                extended_pan_id[5] = 0x63;
                extended_pan_id[6] = 0xF4;
                extended_pan_id[7] = 0xD4;

                esp_zb_set_extended_pan_id(extended_pan_id);
                esp_zb_set_pan_id(0x7CF5);

                ESP_LOGI(TAG, "Joined network successfully (Extended PAN ID: %02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x, PAN ID: 0x%04hx, Channel:%d, Short Address: 0x%04hx)",
                        extended_pan_id[0], extended_pan_id[1], extended_pan_id[2], extended_pan_id[3],
                        extended_pan_id[4], extended_pan_id[5], extended_pan_id[6], extended_pan_id[7],
                        esp_zb_get_pan_id(), esp_zb_get_current_channel(), esp_zb_get_short_address());

                ESP_LOGI(TAG, "Zigbee stack is initialized");
                ESP_ERROR_CHECK(esp_zb_bdb_start_top_level_commissioning(ESP_ZB_BDB_MODE_INITIALIZATION));
            break;

            case ESP_ZB_BDB_SIGNAL_DEVICE_FIRST_START:
            case ESP_ZB_BDB_SIGNAL_DEVICE_REBOOT:
                if(err_status == ESP_OK) {
                    if(esp_zb_bdb_is_factory_new()) {
                        ESP_ERROR_CHECK(esp_zb_bdb_start_top_level_commissioning(ESP_ZB_BDB_MODE_NETWORK_STEERING));
                    }
                }
                else {
                    ESP_LOGW(TAG, "Failed to initialize Zigbee stack (status: %s)", esp_err_to_name(err_status));
                }
            break;

            case ESP_ZB_BDB_SIGNAL_STEERING:
                if(err_status == ESP_OK) {
                    esp_zb_ieee_addr_t extended_pan_id;
                    esp_zb_get_extended_pan_id(extended_pan_id);

                    ESP_LOGI(TAG, "Joined network successfully (Extended PAN ID: %02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x, PAN ID: 0x%04hx, Channel:%d, Short Address: 0x%04hx)",
                        extended_pan_id[7], extended_pan_id[6], extended_pan_id[5], extended_pan_id[4],
                        extended_pan_id[3], extended_pan_id[2], extended_pan_id[1], extended_pan_id[0],
                        esp_zb_get_pan_id(), esp_zb_get_current_channel(), esp_zb_get_short_address());
                }
        break;

        default:
            ESP_LOGI(TAG, "ZDO signal: %s (0x%x), status: %s", esp_zb_zdo_signal_to_string(sig_type), sig_type,
                    esp_err_to_name(err_status));
        break;
    }
}
