#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "twilight";

void app_main(void) {

  ESP_LOGI(TAG, "board booted");

  uint32_t tick = 0;

  while (1) {
    ESP_LOGI(TAG, "alive %lu", tick++);
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}
