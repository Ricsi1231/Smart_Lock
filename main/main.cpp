#include <stdio.h>
#include "zigbee.hpp"
#include "esp_zigbee_core.h"
#include "esp_zigbee_secur.h"
#include "Lock_driver.hpp"

void esp_zb_app_signal_handler(esp_zb_app_signal_t *signal_s);

extern "C" void app_main(void) {
    const char *TAG = "ESP32_LOCK_UART";

    ZigbeeComponent::Zigbee::init();  

    xTaskCreate(ZigbeeComponent::Zigbee::rtosTask, "Zigbee_main", 4096, NULL, 5, NULL);
}

void esp_zb_app_signal_handler(esp_zb_app_signal_t *signal_s) {
    uint32_t *p_signal = signal_s->p_app_signal;
    esp_err_t err_status = signal_s->esp_err_status;
    esp_zb_app_signal_type_t sig_type = static_cast<esp_zb_app_signal_type_t>(*p_signal);

    const char *TAG = "ESP32_LOCK_UART";
        
    //sig_type = ESP_ZB_BDB_SIGNAL_STEERING;
    ESP_LOGI(TAG, "Signal type: %x", sig_type);

    switch(sig_type) {
        case ESP_ZB_ZDO_SIGNAL_SKIP_STARTUP:
            ESP_ERROR_CHECK(esp_zb_bdb_start_top_level_commissioning(ESP_ZB_BDB_MODE_INITIALIZATION));

            ESP_LOGI(TAG, "Zigbee stack is initialized");
        break;

        case ESP_ZB_BDB_SIGNAL_DEVICE_FIRST_START:
        case ESP_ZB_BDB_SIGNAL_DEVICE_REBOOT:
            ESP_ERROR_CHECK(esp_zb_bdb_start_top_level_commissioning(ESP_ZB_BDB_MODE_NETWORK_STEERING));
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
