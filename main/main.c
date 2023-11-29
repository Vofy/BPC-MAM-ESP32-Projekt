#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "led_strip.h"
#include "sdkconfig.h"
#include "led.h"
#include "com.h"
#include "ntp.h"

static const char *TAG = "example";

int period = 10;

void app_main(void)
{
    led_init(&period);
    uart_init(&period);
    wifi_ntp_init();
    print_mac();
}
