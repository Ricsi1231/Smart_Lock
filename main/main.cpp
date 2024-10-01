#include <stdio.h>
#include "zigbee.hpp"
#include "esp_zigbee_core.h"
#include "esp_zigbee_secur.h"
#include "Lock_driver.hpp"
#include "zboss_api_core.h"

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

    esp_zb_ieee_addr_t extended_pan_id;

    const char *TAG = "ESP32_LOCK_UART";
        
    //sig_type = ESP_ZB_BDB_SIGNAL_STEERING;
    ESP_LOGI(TAG, "Signal type: %x", sig_type);

    switch(sig_type) {
        case ESP_ZB_ZDO_SIGNAL_SKIP_STARTUP:
            ESP_LOGI(TAG, "Device skip startup: %x", esp_zb_bdb_start_top_level_commissioning(ESP_ZB_BDB_MODE_INITIALIZATION));

            ESP_LOGI(TAG, "Zigbee stack is initialized");
        break;

        case ESP_ZB_BDB_SIGNAL_DEVICE_FIRST_START:
        case ESP_ZB_BDB_SIGNAL_DEVICE_REBOOT:
            ESP_LOGI(TAG, "Device reboot: %x", esp_zb_bdb_start_top_level_commissioning(ESP_ZB_BDB_MODE_NETWORK_STEERING));
            break;

            case ESP_ZB_BDB_SIGNAL_STEERING:
                if(err_status == ESP_OK) {
                    // esp_zb_af_node_desc_t zigbeeDeviceData;
                    // zigbeeDeviceData.manufacturer_code = 0x55;
                    // zigbeeDeviceData.node_desc_flags = esp_zb_get_short_address();

                    esp_zb_nwk_neighbor_info_t zigbeeDeviceConfig;
                    
                    ESP_LOGI(TAG, "EUI: %x", (int)zigbeeDeviceConfig.ieee_addr);

                    esp_zb_set_node_descriptor_manufacturer_code(0x88);

                    //EUI 64 and manufacturing and model id // 05 - attribute comamnd - 0 endpoint - 1, basic cluster  - 0x0000,  
            
                    esp_zb_get_extended_pan_id(extended_pan_id);

                    ESP_LOGI(TAG, "Joined network successfully (Extended PAN ID: %02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x, PAN ID: 0x%04hx, Channel:%d, Short Address: 0x%04hx)",
                        extended_pan_id[7], extended_pan_id[6], extended_pan_id[5], extended_pan_id[4],
                        extended_pan_id[3], extended_pan_id[2], extended_pan_id[1], extended_pan_id[0],
                        esp_zb_get_pan_id(), esp_zb_get_current_channel(), esp_zb_get_short_address());

                    esp_zb_af_node_desc_t description;

                    description.manufacturer_code = 0x2586;
                    description.node_desc_flags = esp_zb_get_short_address();
                    //description.desc_capability_field = 


                    //esp_zb_set_node_descriptor_manufacturer_code(0x2586);
                }
                else {
                    // esp_zb_ieee_addr_t extended_pan_id;

                    // esp_zb_get_extended_pan_id(extended_pan_id);

                    // ESP_LOGI(TAG, "Joined network successfully (Extended PAN ID: %02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x, PAN ID: 0x%04hx, Channel:%d, Short Address: 0x%04hx)",
                    //     extended_pan_id[7], extended_pan_id[6], extended_pan_id[5], extended_pan_id[4],
                    //     extended_pan_id[3], extended_pan_id[2], extended_pan_id[1], extended_pan_id[0],
                    //     esp_zb_get_pan_id(), esp_zb_get_current_channel(), esp_zb_get_short_address());

                    ESP_LOGI(TAG, "ZIGBEE ERROR %x", err_status);
                }
        break;

        default:
            ESP_LOGI(TAG, "ZDO signal: %s (0x%x), status: %s", esp_zb_zdo_signal_to_string(sig_type), sig_type,
                    esp_err_to_name(err_status));
        break;
    }
}

// void my_zdo_node_desc_callback(esp_zb_zdp_status_t zdo_status, uint16_t addr, esp_zb_af_node_desc_t *node_desc, void *user_ctx) {
//     if (zdo_status == ESP_ZB_ZDP_STATUS_SUCCESS) {
//         printf("ZDO node descriptor request succeeded for node: 0x%04X\n", addr);
        
//         printf("Manufacturer code type: %d, Node id: %d\n", node_desc->manufacturer_code, node_desc->node_desc_flags);
        
//         if (user_ctx != NULL) {
//             //my_custom_context_t *ctx = (my_custom_context_t *)user_ctx;            
//         }
//     } else {
//         printf("ZDO node descriptor request failed with status: %d for node: 0x%04X\n", zdo_status, addr);
//     }
// }
