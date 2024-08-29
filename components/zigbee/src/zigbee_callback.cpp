#include "zigbee.hpp"
#include "esp_zigbee_core.h"

void esp_zb_app_signal_handler(esp_zb_app_signal_t *signal_s) {
        uint32_t *p_signal = signal_s->p_app_signal;
        esp_err_t err_status = signal_s->esp_err_status;
        esp_zb_app_signal_type_t sig_type = static_cast<esp_zb_app_signal_type_t>(*p_signal);

        switch (sig_type) {
            case ESP_ZB_ZDO_SIGNAL_SKIP_STARTUP:
                ESP_LOGI(TAG, "Zigbee stack is initialized");
                start_high_level_commissioning(ESP_ZB_BDB_MODE_INITIALIZATION);
            break;

            case ESP_ZB_BDB_SIGNAL_DEVICE_FIRST_START:
            case ESP_ZB_BDB_SIGNAL_DEVICE_REBOOT:
                if(err_status == ESP_OK) {
                    if(esp_zb_bdb_is_factory_new()) {
                        start_high_level_commissioning(ESP_ZB_BDB_MODE_NETWORK_STEERING);
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
                }
            break;

            default:
                ESP_LOGI(TAG, "ZDO signal: %s (0x%x), status: %s", esp_zb_zdo_signal_to_string(sig_type), sig_type,
                        esp_err_to_name(err_status));
            break;
        }
    }