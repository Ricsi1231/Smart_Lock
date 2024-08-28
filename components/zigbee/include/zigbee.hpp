#pragma once

#include <iostream>
#include "esp_zigbee_core.h"
#include "nvs_flash.h"

#define INSTALLCODE_POLICY_ENABLE       false   
#define ED_AGING_TIMEOUT                ESP_ZB_ED_AGING_TIMEOUT_64MIN
#define ED_KEEP_ALIVE                   3000    
#define HA_ESP_LOCK_ENDPOINT            10     
#define ESP_ZB_PRIMARY_CHANNEL_MASK     ESP_ZB_TRANSCEIVER_ALL_CHANNELS_MASK  

#define ESP_ZB_ZED_CONFIG()                                         \
    {                                                               \
        .esp_zb_role = ESP_ZB_DEVICE_TYPE_ED,                       \
        .install_code_policy = INSTALLCODE_POLICY_ENABLE,           \
        .nwk_cfg = {                                                \
            .zed_cfg = {                                            \
                .ed_timeout = ED_AGING_TIMEOUT,                     \
                .keep_alive = ED_KEEP_ALIVE,                        \
            },                                                      \
        },                                                          \
    }

#define ESP_ZB_DEFAULT_RADIO_CONFIG()                           \
    {                                                           \
        .radio_mode = RADIO_MODE_NATIVE,                        \
    }

#define ESP_ZB_DEFAULT_HOST_CONFIG()                            \
    {                                                           \
        .host_connection_mode = HOST_CONNECTION_MODE_NONE,      \
    }

namespace ZigbeeComponent {
    const char *TAG = "ESP32_LOCK_UART";

    class Zigbee {
        public:
            explicit Zigbee();
            virtual ~Zigbee();

            static void rtosTask(void *pvParameter);
            void init();

        private:
            friend esp_err_t zigbeeAttributeHandler(Zigbee *zigbeeRef);

            void esp_zb_app_signal_handler(esp_zb_app_signal_t *signal_sturct);

            void start_high_level_commissioning(uint8_t mask);
            esp_err_t action_handler(esp_zb_core_action_callback_id_t callback_id, const void *message);
            esp_err_t attribute_handler(const esp_zb_zcl_set_attr_value_message_t *message);
            friend class ZigbeeConfig;
    };

    class ZigbeeConfig {
        public:
            ZigbeeConfig(Zigbee& zigbeeInstance);
            esp_err_t registerActionHandler();
        
        private:
            Zigbee *instance;
    };
}