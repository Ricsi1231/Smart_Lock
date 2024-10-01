#include "zigbee.hpp"
#include "ha/esp_zigbee_ha_standard.h"
#include <cstring>

#ifdef __cplusplus
extern "C"
{
#endif
#include "zboss_api.h"
#include "zboss_api_buf.h"
#ifdef __cplusplus
}
#endif

const char *TAG = "ESP32_LOCK_UART";

LockSystem::Lock ZigbeeComponent::Zigbee::Lock;

namespace ZigbeeComponent {
    void Zigbee::start_high_level_commissioning(uint8_t mask) {
        ESP_ERROR_CHECK(esp_zb_bdb_start_top_level_commissioning(mask));
#include "zboss_api_buf.h"
    }
    
    esp_err_t Zigbee::attribute_handler(const esp_zb_zcl_set_attr_value_message_t *message) {
        if(message->info.dst_endpoint == HA_ESP_LOCK_ENDPOINT && message->info.cluster == ESP_ZB_ZCL_CLUSTER_ID_ON_OFF && message->attribute.id == ESP_ZB_ZCL_ATTR_ON_OFF_ON_OFF_ID) {
            if(message->attribute.data.type == ESP_ZB_ZCL_ATTR_ON_OFF_ON_OFF_ID && message->attribute.data.type == ESP_ZB_ZCL_ATTR_TYPE_BOOL) {
                ESP_LOGI(TAG, "Message arrived from Zigbee dongle");   

                Lock.setKey();
                Lock.open();
            }
            else {
                ESP_LOGI(TAG, "Wrong Lock command");
            }
        }
        else if (message->info.dst_endpoint == HA_ESP_LOCK_ENDPOINT && message->info.cluster == 0x0000 && message->attribute.id == 0x0005) {

        }

        return ESP_OK;
    }

        esp_err_t Zigbee::action_handler(esp_zb_core_action_callback_id_t callback_id, const void *message) {
        esp_err_t error_status = ESP_OK;

        ESP_LOGI(TAG, "Data: %d", callback_id);

        switch(callback_id) {
            case ESP_ZB_CORE_SET_ATTR_VALUE_CB_ID:
                error_status = attribute_handler((esp_zb_zcl_set_attr_value_message_t *)message);
            break;

            default:
                ESP_LOGW(TAG, "Receive Zigbee action(0x%x) callback", callback_id);
            break;
        }

        return error_status;
    }

    bool Zigbee::esp_zb_zcl_raw_command_callback(uint8_t bufId) {
        ESP_LOGI(TAG, "Zigbee raw command callback");
        ESP_LOGI(TAG, "Zigbee raw command callback enpoint: %d", bufId);
        
        uint8_t buf[sizeof(bufId)];
        zb_zcl_attr_addr_info_t *cmd_info = (zb_zcl_attr_addr_info_t *)zb_buf_get_tail(bufId, sizeof(zb_zcl_attr_addr_info_t));
        
        printf("cluster id: 0x%x, attributte> %d\n", cmd_info->cluster_id, cmd_info->attr_id);
        
        memcpy(buf, zb_buf_begin(bufId), sizeof(buf));
        
        ESP_LOGI("RAW", "bufid: %d size: %d", bufId, sizeof(buf));
        
        for (int i = 0; i < sizeof(buf); ++i) {
            printf("Data: 0x%02X, Data Size: %d ", buf[i], i);
        }

        printf("\n");

        if(cmd_info->cluster_id == 0x01 && cmd_info->attr_id == 0x00 && buf[0] == 0x01) {
            Lock.open();
            
            ESP_LOGI(TAG, "Correct data received for open a lock");
        }

        return false;
    }

    void Zigbee::rtosTask(void *pvParameters) {
        char modelid[] = {13, 'E', 'S', 'P', '3', '2', 'C', '6', '-', 'L', 'o', 'c', 'k'};
        char manufname[] = {9, 'E', 's', 'p', 'r', 'e', 's', 's', 'i', 'f'};

        esp_zb_cfg_t zb_nwk_cfg = ESP_ZB_ZED_CONFIG();
        esp_zb_init(&zb_nwk_cfg);
        esp_zb_set_primary_network_channel_set(ESP_ZB_PRIMARY_CHANNEL_MASK);

        uint8_t test_attr, test_attr2; 
    
        esp_zb_endpoint_config_t endPoint;

        endPoint.endpoint = HA_ESP_LOCK_ENDPOINT;
        endPoint.app_profile_id = 0x0104;
        endPoint.app_device_id = 0x0005;

        test_attr = 0;
        test_attr2 = 4;

        //! BASIC AND ON - OFF CLUSTER CONFIGURATION

        esp_zb_attribute_list_t *esp_zb_basic_cluster = esp_zb_zcl_attr_list_create(ESP_ZB_ZCL_CLUSTER_ID_BASIC);
        
        // // esp_zb_cluster_add_attr(doorCluster, ESP_ZB_ZCL_ATTR_DOOR_LOCK_LOCK_STATE_ID, test_attr);

        esp_zb_basic_cluster_add_attr(esp_zb_basic_cluster, ESP_ZB_ZCL_ATTR_BASIC_ZCL_VERSION_ID, &test_attr);
        esp_zb_basic_cluster_add_attr(esp_zb_basic_cluster, ESP_ZB_ZCL_ATTR_BASIC_POWER_SOURCE_ID, &test_attr2);
        
        esp_zb_cluster_update_attr(esp_zb_basic_cluster, ESP_ZB_ZCL_ATTR_BASIC_ZCL_VERSION_ID, &test_attr2);
        
        esp_zb_basic_cluster_add_attr(esp_zb_basic_cluster, ESP_ZB_ZCL_ATTR_BASIC_MODEL_IDENTIFIER_ID, &modelid[0]);
        esp_zb_basic_cluster_add_attr(esp_zb_basic_cluster, ESP_ZB_ZCL_ATTR_BASIC_MANUFACTURER_NAME_ID, &manufname[0]);
        
        esp_zb_attribute_list_t *esp_zb_identify_cluster = esp_zb_zcl_attr_list_create(ESP_ZB_ZCL_CLUSTER_ID_IDENTIFY);
        esp_zb_identify_cluster_add_attr(esp_zb_identify_cluster, ESP_ZB_ZCL_ATTR_IDENTIFY_IDENTIFY_TIME_ID, &test_attr);
        
        esp_zb_attribute_list_t *esp_zb_groups_cluster = esp_zb_zcl_attr_list_create(ESP_ZB_ZCL_CLUSTER_ID_GROUPS);
        esp_zb_groups_cluster_add_attr(esp_zb_groups_cluster, ESP_ZB_ZCL_ATTR_GROUPS_NAME_SUPPORT_ID, &test_attr);
        
        esp_zb_attribute_list_t *esp_zb_scenes_cluster = esp_zb_scenes_cluster_create(NULL);
        esp_zb_cluster_update_attr(esp_zb_scenes_cluster, ESP_ZB_ZCL_ATTR_SCENES_NAME_SUPPORT_ID, &test_attr);
        
        esp_zb_on_off_cluster_cfg_t on_off_cfg;
        on_off_cfg.on_off = ESP_ZB_ZCL_ON_OFF_ON_OFF_DEFAULT_VALUE;

        esp_zb_attribute_list_t *esp_zb_on_off_cluster = esp_zb_on_off_cluster_create(&on_off_cfg);
        esp_zb_cluster_list_t *esp_zb_cluster_list = esp_zb_zcl_cluster_list_create();

        esp_zb_cluster_list_add_basic_cluster(esp_zb_cluster_list, esp_zb_basic_cluster, ESP_ZB_ZCL_CLUSTER_SERVER_ROLE);
        
        esp_zb_cluster_list_add_identify_cluster(esp_zb_cluster_list, esp_zb_identify_cluster, ESP_ZB_ZCL_CLUSTER_SERVER_ROLE);
        
        esp_zb_cluster_list_add_groups_cluster(esp_zb_cluster_list, esp_zb_groups_cluster, ESP_ZB_ZCL_CLUSTER_SERVER_ROLE);
        
        esp_zb_cluster_list_add_scenes_cluster(esp_zb_cluster_list, esp_zb_scenes_cluster, ESP_ZB_ZCL_CLUSTER_SERVER_ROLE);
        
        esp_zb_cluster_list_add_on_off_cluster(esp_zb_cluster_list, esp_zb_on_off_cluster, ESP_ZB_ZCL_CLUSTER_SERVER_ROLE);

        //! BASIC AND ON - OFF CLUSTER CONFIGURATION

        //! LOCK CLUSTER CONFIGURATION

        esp_zb_attribute_list_t* doorClusterAttributes = esp_zb_zcl_attr_list_create(ESP_ZB_ZCL_CLUSTER_ID_DOOR_LOCK);
        
        esp_zb_door_lock_cluster_cfg_t doorLockClusterConfig = {
            .lock_state = 0x05,
            .lock_type = 0x01,
            .actuator_enabled = true
        };

        esp_zb_door_lock_cluster_add_attr(doorClusterAttributes, ESP_ZB_ZCL_ATTR_DOOR_LOCK_LOCK_STATE_ID, &test_attr);
        esp_zb_door_lock_cluster_add_attr(doorClusterAttributes, ESP_ZB_ZCL_ATTR_DOOR_LOCK_LOCK_TYPE_ID, &test_attr);
        esp_zb_door_lock_cluster_add_attr(doorClusterAttributes, ESP_ZB_ZCL_ATTR_DOOR_LOCK_ACTUATOR_ENABLED_ID, &test_attr);

        esp_zb_attribute_list_t *doorLockCluster = esp_zb_door_lock_cluster_create(&doorLockClusterConfig);

        esp_zb_cluster_list_add_on_off_cluster(esp_zb_cluster_list, doorLockCluster, ESP_ZB_ZCL_CLUSTER_SERVER_ROLE);

        //! LOCK CLUSTER CONFIGURATION

        //! CLUSTER LIST CONFIGURATION AND CALLBACK FUNCTION REGISTRATION

        esp_zb_ep_list_t *esp_zb_ep_list = esp_zb_ep_list_create();
        
        esp_zb_ep_list_add_ep(esp_zb_ep_list, esp_zb_cluster_list, endPoint);
        esp_zb_device_register(esp_zb_ep_list);
        
        esp_zb_raw_command_handler_register(esp_zb_zcl_raw_command_callback);
        esp_zb_core_action_handler_register(action_handler);
        // // esp_zb_identify_notify_handler_register(HA_ESP_LOCK_ENDPOINT, esp_zb_zcl_reset_default_attr_callback);
        
        //! CLUSTER LIST CONFIGURATION AND CALLBACK FUNCTION REGISTRATION

        ESP_ERROR_CHECK(esp_zb_start(false));
        
        esp_zb_main_loop_iteration();
    }

    // void Zigbee::rtosTask(void *pvParameter) {
    //     char modelid[] = {13, 'E', 'S', 'P', '3', '2', 'C', '6', '.', 'L', 'i', 'g', 'h', 't'};
    //     char manufname[] = {9, 'E', 's', 'p', 'r', 'e', 's', 's', 'i', 'f'};

    //     esp_zb_cfg_t zigbee_network_config = ESP_ZB_ZED_CONFIG();
    //     esp_zb_init(&zigbee_network_config);

    //     esp_zb_door_lock_cfg_t doorLockConfigure = ESP_ZB_DEFAULT_DOOR_LOCK_CONFIG();
    //     esp_zb_ep_list_t  *ep_list = esp_zb_door_lock_ep_create(HA_ESP_LOCK_ENDPOINT, &doorLockConfigure);

    //     esp_zb_device_register(ep_list);
    //     esp_zb_core_action_handler_register(action_handler);
    //     esp_zb_set_primary_network_channel_set(ESP_ZB_PRIMARY_CHANNEL_MASK);

    //     esp_zb_attribute_list_t *esp_zb_basic_cluster = esp_zb_zcl_attr_list_create(ESP_ZB_ZCL_CLUSTER_ID_BASIC);
    //     esp_zb_basic_cluster->attribute = 0x0005;
    //     esp_zb_basic_cluster->cluster_id = 0x0000;
    //     esp_zb_basic_cluster->next = nullptr;


    // esp_zb_basic_cluster_add_attr(esp_zb_basic_cluster, ESP_ZB_ZCL_ATTR_BASIC_MODEL_IDENTIFIER_ID, &modelid[0]);

    //         esp_zb_basic_cluster_add_attr(esp_zb_basic_cluster, ESP_ZB_ZCL_ATTR_BASIC_MANUFACTURER_NAME_ID, &manufname[0]);

    //         esp_zb_attribute_list_t *esp_zb_identify_cluster = esp_zb_zcl_attr_list_create(ESP_ZB_ZCL_CLUSTER_ID_IDENTIFY);

    //     ESP_ERROR_CHECK(esp_zb_start(false));

    //     Lock.init(UART_RX_PIN, UART_TX_PIN);

    //     esp_zb_main_loop_iteration();
    // }

    void Zigbee::init() {
        esp_zb_platform_config_t zigbee_config = {
            .radio_config = ESP_ZB_DEFAULT_RADIO_CONFIG(),
            .host_config = ESP_ZB_DEFAULT_HOST_CONFIG(),
        };

        Lock.init(UART_RX_PIN, UART_TX_PIN);
        Lock.setKey();

        ESP_ERROR_CHECK(nvs_flash_init());
        ESP_ERROR_CHECK(esp_zb_platform_config(&zigbee_config));
    }
}