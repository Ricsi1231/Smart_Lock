
idf_component_register(
    SRCS "src/lock_driver.cpp"  
    INCLUDE_DIRS "include"   
    REQUIRES driver uart_driver
)
