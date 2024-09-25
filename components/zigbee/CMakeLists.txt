idf_component_register (
    SRCS "src/zigbee.cpp"
    INCLUDE_DIRS "include"  
    REQUIRES esp-zigbee-lib nvs_flash Lock_Driver
)