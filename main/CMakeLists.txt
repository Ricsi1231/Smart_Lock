idf_component_register(SRCS "main.cpp"
                    INCLUDE_DIRS "."
                    REQUIRES zigbee Lock_Driver esp-zigbee-lib esp-zboss-lib
)