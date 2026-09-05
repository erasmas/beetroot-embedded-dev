#include "calibration.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "light_sensor.h"

static const char *TAG = "twilight";

#define KEY_GPIO GPIO_NUM_5
#define SAMPLE_PERIOD_MS 500

static void key_set(bool on) { gpio_set_level(KEY_GPIO, on); }

static void key_init(void) {
  gpio_reset_pin(KEY_GPIO);
  gpio_set_direction(KEY_GPIO, GPIO_MODE_OUTPUT);
  key_set(false);
}

void app_main(void) {
  key_init();
  light_sensor_init();
  calibration_init(key_set);

  bool relay_on = false;
  bool was_on = false;

  while (1) {
    if (calibration_handle_input(SAMPLE_PERIOD_MS)) {
      continue;
    }

    int raw = light_sensor_read();

    if (!calibration_active()) {
      if (raw < calibration_threshold_dark()) {
        relay_on = true;
      } else if (raw > calibration_threshold_light()) {
        relay_on = false;
      }

      key_set(relay_on);

      if (relay_on != was_on) {
        ESP_LOGW(TAG, "relay %s at raw=%d", relay_on ? "ON" : "OFF", raw);
        was_on = relay_on;
      }
    }

    ESP_LOGI(TAG, "state=%s raw=%d, v=%4d mV, dark=%d, light=%d, relay=%s",
             calibration_state_name(), raw, light_sensor_to_millivolts(raw),
             calibration_threshold_dark(), calibration_threshold_light(),
             relay_on ? "ON" : "OFF");
  }
}
