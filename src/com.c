/* UART Echo Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
#include "esp_log.h"
#include "com.h"
#include <string.h>
#include <ctype.h>
#include "time.h"
#include "esp_mac.h"

#include "ntp.h"
#include "random_gen.h"

static const char *TAG = "UART TEST";

#define BUF_SIZE (1024)

static void uart_com(int *period)
{
    uart_config_t uart_config = {
        .baud_rate = ECHO_UART_BAUD_RATE,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };
    int intr_alloc_flags = 0;

#if CONFIG_UART_ISR_IN_IRAM
    intr_alloc_flags = ESP_INTR_FLAG_IRAM;
#endif

    ESP_ERROR_CHECK(uart_driver_install(ECHO_UART_PORT_NUM, BUF_SIZE * 2, BUF_SIZE * 2, 2, NULL, intr_alloc_flags));
    ESP_ERROR_CHECK(uart_param_config(ECHO_UART_PORT_NUM, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(ECHO_UART_PORT_NUM, ECHO_TEST_TXD, ECHO_TEST_RXD, ECHO_TEST_RTS, ECHO_TEST_CTS));

    uint8_t *data = (uint8_t *)malloc(BUF_SIZE);

    while (1)
    {
        int len = uart_read_bytes(ECHO_UART_PORT_NUM, data, (BUF_SIZE - 1), 20 / portTICK_PERIOD_MS);

        if (len)
        {
            if (!strncmp((char *)data, "PER:", 4) && len == 7)
            {
                char *chperiod = malloc(3 * sizeof(char));
                strncpy(chperiod, (char *)(data + 4), 3);

                if (isdigit((u_char)*chperiod) && isdigit((u_char) * (chperiod + 1)) && isdigit((u_char) * (chperiod + 2)))
                {
                    *period = atoi(chperiod);
                }

                char *response = (char *)malloc(24 * sizeof(char));
                sprintf(response, "Entered period is %.1f", (float)*period / 10);
                uart_write_bytes(ECHO_UART_PORT_NUM, response, strlen(response));
            }
            else if (!strncmp((char *)data, "UNIXTIME?", 9) && len == 9)
            {
                struct tm *timeinfo = (struct tm *)get_time();
                char strftime_buf[64];
                strftime(strftime_buf, sizeof(strftime_buf), "%llx", timeinfo);
                uart_write_bytes(ECHO_UART_PORT_NUM, strftime_buf, strlen(strftime_buf));
            }
            else if (!strncmp((char *)data, "RANDOM?", 7))
            {
                char *response = (char *)malloc(16 * sizeof(char));
                sprintf(response, "%llx", get_random());
                uart_write_bytes(ECHO_UART_PORT_NUM, response, strlen(response));
            }
            else if (!strncmp((char *)data, "MAC?", 4))
            {
                uint64_t chip_mac = 0LL;
    esp_efuse_mac_get_default((uint8_t *)(&chip_mac));

    char *response = (char *)malloc(16 * sizeof(char));
    sprintf(response, "%llx", chip_mac);
    uart_write_bytes(ECHO_UART_PORT_NUM, response, strlen(response));
            }
            else
            {
                char *response = "Invalid input";
                uart_write_bytes(ECHO_UART_PORT_NUM, response, strlen(response));

                uart_write_bytes(ECHO_UART_PORT_NUM, data, BUF_SIZE);
            }
        }
    }
}

void print_mac(void)
{
    uint64_t chip_mac = 0LL;
    esp_efuse_mac_get_default((uint8_t *)(&chip_mac));

    char *response = (char *)malloc(16 * sizeof(char));
    sprintf(response, "%llx", chip_mac);
    uart_write_bytes(ECHO_UART_PORT_NUM, response, strlen(response));
}

void uart_init(int *period)
{
    xTaskCreate(uart_com, "uart_com_task", ECHO_TASK_STACK_SIZE, period, 10, NULL);
}