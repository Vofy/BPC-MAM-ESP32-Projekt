#include "stdio.h"
#include "stdint.h"
#include "esp_random.h"

uint64_t get_random(void) {

    uint64_t buff;
    esp_fill_random(&buff,sizeof(uint64_t));

    return buff;
}