#include "light_sensor.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_log.h"
#include "esp_timer.h"

static const char *TAG = "adc";

#define LDR_CHANNEL ADC_CHANNEL_3

// A single sample is a phase sample of whatever hum sits on the divider node,
// so integrate across several mains periods instead.
#define MAINS_WINDOW_US 100000

static adc_oneshot_unit_handle_t adc;
static adc_cali_handle_t cali;

void light_sensor_init(void) {
  adc_oneshot_unit_init_cfg_t unit_cfg = {
      .unit_id = ADC_UNIT_1,
  };
  ESP_ERROR_CHECK(adc_oneshot_new_unit(&unit_cfg, &adc));

  adc_oneshot_chan_cfg_t chan_cfg = {
      .atten = ADC_ATTEN_DB_12,
      .bitwidth = ADC_BITWIDTH_DEFAULT,
  };
  ESP_ERROR_CHECK(adc_oneshot_config_channel(adc, LDR_CHANNEL, &chan_cfg));

  adc_cali_curve_fitting_config_t cali_cfg = {
      .unit_id = ADC_UNIT_1,
      .chan = LDR_CHANNEL,
      .atten = ADC_ATTEN_DB_12,
      .bitwidth = ADC_BITWIDTH_DEFAULT,
  };
  ESP_ERROR_CHECK(adc_cali_create_scheme_curve_fitting(&cali_cfg, &cali));

  ESP_LOGI(TAG, "ADC1 channel %d, averaging over %d us", LDR_CHANNEL,
           MAINS_WINDOW_US);
}

int light_sensor_read(void) {
  int64_t sum = 0;
  int samples = 0;
  int64_t deadline = esp_timer_get_time() + MAINS_WINDOW_US;

  while (esp_timer_get_time() < deadline) {
    int raw;
    ESP_ERROR_CHECK(adc_oneshot_read(adc, LDR_CHANNEL, &raw));
    sum += raw;
    samples++;
  }

  return (int)(sum / samples);
}

int light_sensor_to_millivolts(int raw) {
  int millivolts;
  ESP_ERROR_CHECK(adc_cali_raw_to_voltage(cali, raw, &millivolts));
  return millivolts;
}
