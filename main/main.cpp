#include <stdio.h>
#include "lock_driver.hpp"
#include "zigbee.hpp"

#define UART_PORT (UART_NUM_1)
#define UART_RX_PIN (GPIO_NUM_4)
#define UART_TX_PIN (GPIO_NUM_5)

LockSystem::Lock Lock;

extern "C" void app_main(void) {

    Lock.initalize(UART_PORT, UART_RX_PIN, UART_TX_PIN);
    ZigbeeComponent::Zigbee::init();
    
    Lock.open();
}
