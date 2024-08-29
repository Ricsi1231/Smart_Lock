#include "zigbee.hpp"
#include "ha/esp_zigbee_ha_standard.h"

const char *TAG = "ESP32_LOCK_UART";

namespace ZigbeeComponent {
    void Zigbee::start_high_level_commissioning(uint8_t mask) {
        ESP_ERROR_CHECK(esp_zb_bdb_start_top_level_commissioning(mask));
    }
    
        esp_err_t Zigbee::attribute_handler(const esp_zb_zcl_set_attr_value_message_t *message) {
        esp_err_t error_state = ESP_OK;

        if(message->info.dst_endpoint == HA_ESP_LOCK_ENDPOINT && message->info.cluster == ESP_ZB_ZCL_CLUSTER_ID_ON_OFF) {
            if(message->attribute.id == ESP_ZB_ZCL_ATTR_ON_OFF_ON_OFF_ID && message->attribute.data.type == ESP_ZB_ZCL_ATTR_TYPE_BOOL) {
                vTaskDelay(10);
            }
        }

        return error_state;
    }

        esp_err_t Zigbee::action_handler(esp_zb_core_action_callback_id_t callback_id, const void *message) {
        esp_err_t error_status = ESP_OK;

        switch (callback_id) {
            case ESP_ZB_CORE_SET_ATTR_VALUE_CB_ID:
                error_status = attribute_handler((esp_zb_zcl_set_attr_value_message_t *)message);
            break;

            default:
                ESP_LOGW(TAG, "Receive Zigbee action(0x%x) callback", callback_id);
            break;
        }

        return error_status;
    }

    void Zigbee::rtosTask(void *pvParameter) {
        esp_zb_cfg_t zigbee_network_config = ESP_ZB_ZED_CONFIG();
        esp_zb_init(&zigbee_network_config);

        esp_zb_on_off_light_cfg_t on_off_cfg = ESP_ZB_DEFAULT_ON_OFF_LIGHT_CONFIG();
        esp_zb_ep_list_t  *ep_list = esp_zb_on_off_light_ep_create(HA_ESP_LOCK_ENDPOINT, &on_off_cfg);

        esp_zb_device_register(ep_list);
        esp_zb_core_action_handler_register(action_handler);
        esp_zb_set_primary_network_channel_set(ESP_ZB_PRIMARY_CHANNEL_MASK);

        ESP_ERROR_CHECK(esp_zb_start(false));

        esp_zb_main_loop_iteration();
    }

    void Zigbee::init() {
        esp_zb_platform_config_t zigbee_config = {
            .radio_config = ESP_ZB_DEFAULT_RADIO_CONFIG(),
            .host_config = ESP_ZB_DEFAULT_HOST_CONFIG(),
        };

        ESP_ERROR_CHECK(nvs_flash_init());
        ESP_ERROR_CHECK(esp_zb_platform_config(&zigbee_config));
    }
}

