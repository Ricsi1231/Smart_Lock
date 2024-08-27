idf_component_register(
    SRCS "src/lock_driver.cpp"  
    INCLUDE_DIRS "include"   
    REQUIRES esp_driver_uart esp_driver_gpio uart_driver
)
