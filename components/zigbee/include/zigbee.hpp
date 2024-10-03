#pragma once

#include <iostream>
#include "nvs_flash.h"
#include "esp_zigbee_core.h"
#include "Lock_driver.hpp"

#define INSTALLCODE_POLICY_ENABLE       false   
#define ED_AGING_TIMEOUT                ESP_ZB_ED_AGING_TIMEOUT_64MIN
#define ED_KEEP_ALIVE                   3000    
#define HA_ESP_LOCK_ENDPOINT            1
#define ESP_ZB_PRIMARY_CHANNEL_MASK     ESP_ZB_TRANSCEIVER_ALL_CHANNELS_MASK  

#define UART_RX_PIN (GPIO_NUM_4)
#define UART_TX_PIN (GPIO_NUM_5)

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
        .radio_mode = ZB_RADIO_MODE_NATIVE,                        \
    }

#define ESP_ZB_DEFAULT_HOST_CONFIG()                            \
    {                                                           \
        .host_connection_mode = ZB_HOST_CONNECTION_MODE_NONE,      \
    }

#define UART_PORT (UART_NUM_1)
#define UART_RX_PIN (GPIO_NUM_4)
#define UART_TX_PIN (GPIO_NUM_5)

namespace ZigbeeComponent {
    class Zigbee {
        public:
            Zigbee() = delete;
            ~Zigbee() = delete;
            Zigbee(const Zigbee&) = delete;
            Zigbee& operator=(const Zigbee&) = delete;
            Zigbee(Zigbee&&) = delete;
            Zigbee& operator=(Zigbee&&) = delete;

            static void rtosTask(void *pvParameter);
            static void init();
            // static void sendCb(esp_zb_zdp_status_t zdo_status, uint16_t addr, uint8_t endpoint, void *user_ctx);

        private:
            static void start_high_level_commissioning(uint8_t mask);
            static esp_err_t attribute_handler(const esp_zb_zcl_set_attr_value_message_t *message);
            static esp_err_t action_handler(esp_zb_core_action_callback_id_t callback_id, const void *message);
            static bool esp_zb_zcl_raw_command_callback(uint8_t bufId);
            static void sendZclStatus(esp_zb_zcl_command_send_status_message_t message);
            //static zb_buf_ent_t *esp_zb_buf_get_by_id(zb_bufid_t buf); 
            //static zb_uint8_t *esp_zb_buf_get_tail(zb_bufid_t buf, zb_uint16_t size); 

            static LockSystem::Lock Lock;
    };
}