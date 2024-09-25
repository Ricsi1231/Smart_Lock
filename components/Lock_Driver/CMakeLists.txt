idf_component_register(
    SRCS "src/Lock_driver.cpp"  
    INCLUDE_DIRS "include"   
    REQUIRES driver uart_driver
)
