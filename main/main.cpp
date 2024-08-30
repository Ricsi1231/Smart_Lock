#include <stdio.h>
#include "lock_driver.hpp"
#include "zigbee.hpp"
#include "esp_zigbee_core.h"

#define UART_PORT (UART_NUM_1)
#define UART_RX_PIN (GPIO_NUM_4)
#define UART_TX_PIN (GPIO_NUM_5)

LockSystem::Lock Lock;

void esp_zb_app_signal_handler(esp_zb_app_signal_t *signal_s);

extern "C" void app_main(void) {
    const char *TAG = "ESP32_LOCK_UART";

    ZigbeeComponent::Zigbee::init();
    Lock.initalize(UART_PORT, UART_RX_PIN, UART_TX_PIN);

    esp_zb_ieee_addr_t extended_pan_id;
                    esp_zb_get_extended_pan_id(extended_pan_id);
                    ESP_LOGI(TAG, "Joined network successfully (Extended PAN ID: %02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x, PAN ID: 0x%04hx, Channel:%d, Short Address: 0x%04hx)",
                        extended_pan_id[7], extended_pan_id[6], extended_pan_id[5], extended_pan_id[4],
                        extended_pan_id[3], extended_pan_id[2], extended_pan_id[1], extended_pan_id[0],
                        esp_zb_get_pan_id(), esp_zb_get_current_channel(), esp_zb_get_short_address());
    
    //Lock.open();
    //xTaskCreate(ZigbeeComponent::Zigbee::rtosTask, "Zigbee_main", 4096, NULL, 5, NULL);
}

void esp_zb_app_signal_handler(esp_zb_app_signal_t *signal_s) {
        uint32_t *p_signal = signal_s->p_app_signal;
        esp_err_t err_status = signal_s->esp_err_status;
        esp_zb_app_signal_type_t sig_type = static_cast<esp_zb_app_signal_type_t>(*p_signal);

        const char *TAG = "ESP32_LOCK_UART";

        switch (sig_type) {
            case ESP_ZB_ZDO_SIGNAL_SKIP_STARTUP:
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
