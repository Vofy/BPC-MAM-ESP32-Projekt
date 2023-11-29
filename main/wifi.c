#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_dpp.h"
#include "esp_log.h"

#define WIFI_AP_NAME "PokojNet"
#define WIFI_AP_PASS "sejra123"

void wifi_init(void) {
    //const wifi_init_config_t *config = WIFI_INIT_CONFIG_DEFAULT();
    wifi_config_t conf = {
        .sta = {
            .ssid = WIFI_AP_NAME,
            .password = WIFI_AP_PASS,
        },
    };
    esp_wifi_init(&conf);
    //esp_wifi_set_mode(WIFI_MODE_STA);

    esp_wifi_init(&conf);
    esp_wifi_set_config(WIFI_IF_STA, &conf);
    esp_wifi_connect();
}