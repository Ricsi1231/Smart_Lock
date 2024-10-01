idf_component_register (
    INCLUDE_DIRS "include"
    SRCS "src/zigbee.cpp"      
    REQUIRES esp-zigbee-lib nvs_flash Lock_Driver espressif__esp-zboss-lib
)