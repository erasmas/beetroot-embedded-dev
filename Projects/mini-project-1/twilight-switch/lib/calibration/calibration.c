#include "calibration.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "light_sensor.h"
#include "nvs.h"
#include "nvs_flash.h"

static const char *TAG = "calibration";

#define BUTTON_GPIO GPIO_NUM_0

#define DEFAULT_THRESHOLD_DARK 1500
#define DEFAULT_THRESHOLD_LIGHT 2500
#define MIN_THRESHOLD_GAP 300

#define BUTTON_POLL_MS 20
#define BUTTON_DEBOUNCE_POLLS 3
#define LONG_PRESS_MS 2000
#define BLINK_MS 120
#define BUTTON_QUEUE_LENGTH 4

// The key doubles as the calibration indicator, so de-energise it and let the
// photoresistor recover before capturing a threshold.
#define CAPTURE_SETTLE_MS 200

#define NVS_NAMESPACE "twilight"
#define NVS_KEY_DARK "dark_raw"
#define NVS_KEY_LIGHT "light_raw"

typedef enum {
  BUTTON_SHORT_PRESS,
  BUTTON_LONG_PRESS,
} button_event_t;

typedef enum {
  STATE_NORMAL,
  STATE_AWAIT_DARK,
  STATE_AWAIT_LIGHT,
} calibration_state_t;

static void (*set_key)(bool on);
static QueueHandle_t button_events;
static calibration_state_t state = STATE_NORMAL;
static int threshold_dark = DEFAULT_THRESHOLD_DARK;
static int threshold_light = DEFAULT_THRESHOLD_LIGHT;
static int captured_dark;

// The captured values are the extremes the sensor reaches, so switching at
// them would need light beyond anything the user demonstrated. Put the trip
// points inside the span instead.
static void derive_thresholds(int dark_raw, int light_raw) {
  int span = light_raw - dark_raw;
  threshold_dark = dark_raw + span / 3;
  threshold_light = dark_raw + (2 * span) / 3;
}

static const char *name_of(calibration_state_t value) {
  switch (value) {
  case STATE_AWAIT_DARK:
    return "AWAIT_DARK";
  case STATE_AWAIT_LIGHT:
    return "AWAIT_LIGHT";
  default:
    return "NORMAL";
  }
}

static void storage_init(void) {
  esp_err_t err = nvs_flash_init();
  if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    ESP_ERROR_CHECK(nvs_flash_erase());
    err = nvs_flash_init();
  }
  ESP_ERROR_CHECK(err);
}

static void thresholds_load(void) {
  nvs_handle_t nvs;
  int32_t dark;
  int32_t light;

  if (nvs_open(NVS_NAMESPACE, NVS_READONLY, &nvs) == ESP_OK) {
    if (nvs_get_i32(nvs, NVS_KEY_DARK, &dark) == ESP_OK &&
        nvs_get_i32(nvs, NVS_KEY_LIGHT, &light) == ESP_OK) {
      derive_thresholds(dark, light);
      ESP_LOGW(TAG,
               "loaded from NVS: captured dark=%d light=%d -> thresholds "
               "dark=%d light=%d",
               (int)dark, (int)light, threshold_dark, threshold_light);
      nvs_close(nvs);
      return;
    }
    nvs_close(nvs);
  }

  ESP_LOGW(TAG, "no saved thresholds, using defaults dark=%d light=%d",
           threshold_dark, threshold_light);
}

static bool thresholds_save(int dark_raw, int light_raw) {
  nvs_handle_t nvs;
  esp_err_t err = nvs_open(NVS_NAMESPACE, NVS_READWRITE, &nvs);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "cannot open NVS for writing: %s", esp_err_to_name(err));
    return false;
  }

  err = nvs_set_i32(nvs, NVS_KEY_DARK, dark_raw);
  if (err == ESP_OK) {
    err = nvs_set_i32(nvs, NVS_KEY_LIGHT, light_raw);
  }
  if (err == ESP_OK) {
    err = nvs_commit(nvs);
  }
  nvs_close(nvs);

  if (err != ESP_OK) {
    ESP_LOGE(TAG, "cannot save thresholds: %s", esp_err_to_name(err));
    return false;
  }
  return true;
}

static void button_init(void) {
  gpio_config_t button_config = {
      .pin_bit_mask = 1ULL << BUTTON_GPIO,
      .mode = GPIO_MODE_INPUT,
      .pull_up_en = GPIO_PULLUP_ENABLE,
      .pull_down_en = GPIO_PULLDOWN_DISABLE,
      .intr_type = GPIO_INTR_DISABLE,
  };
  ESP_ERROR_CHECK(gpio_config(&button_config));
}

static void button_task(void *arg) {
  int stable_level = 1;
  int candidate_level = 1;
  int candidate_polls = 0;
  int64_t pressed_at = 0;
  bool long_press_sent = false;

  while (1) {
    int level = gpio_get_level(BUTTON_GPIO);

    if (level != candidate_level) {
      candidate_level = level;
      candidate_polls = 0;
    } else if (candidate_polls < BUTTON_DEBOUNCE_POLLS) {
      candidate_polls++;
    }

    if (candidate_polls >= BUTTON_DEBOUNCE_POLLS &&
        candidate_level != stable_level) {
      stable_level = candidate_level;
      if (stable_level == 0) {
        pressed_at = esp_timer_get_time();
        long_press_sent = false;
      } else if (!long_press_sent) {
        button_event_t event = BUTTON_SHORT_PRESS;
        xQueueSend(button_events, &event, 0);
      }
    }

    if (stable_level == 0 && !long_press_sent &&
        esp_timer_get_time() - pressed_at >= LONG_PRESS_MS * 1000) {
      long_press_sent = true;
      button_event_t event = BUTTON_LONG_PRESS;
      xQueueSend(button_events, &event, 0);
    }

    vTaskDelay(pdMS_TO_TICKS(BUTTON_POLL_MS));
  }
}

static void blink(int times) {
  for (int i = 0; i < times; i++) {
    set_key(true);
    vTaskDelay(pdMS_TO_TICKS(BLINK_MS));
    set_key(false);
    vTaskDelay(pdMS_TO_TICKS(BLINK_MS));
  }
}

static int capture(void) {
  set_key(false);
  vTaskDelay(pdMS_TO_TICKS(CAPTURE_SETTLE_MS));
  return light_sensor_read();
}

static void handle_long_press(void) {
  if (state == STATE_NORMAL) {
    ESP_LOGW(TAG, "started; darken the sensor, then short press to capture the "
                  "dark point");
    blink(3);
    set_key(true);
    state = STATE_AWAIT_DARK;
    return;
  }

  ESP_LOGW(TAG, "aborted in %s; keeping dark=%d light=%d", name_of(state),
           threshold_dark, threshold_light);
  blink(1);
  state = STATE_NORMAL;
}

static void handle_short_press(void) {
  int captured_light;

  switch (state) {
  case STATE_AWAIT_DARK:
    captured_dark = capture();
    ESP_LOGW(TAG, "captured dark=%d; light the sensor, then short press to "
                  "capture the light point",
             captured_dark);
    blink(1);
    set_key(false);
    state = STATE_AWAIT_LIGHT;
    break;

  case STATE_AWAIT_LIGHT:
    captured_light = capture();
    if (captured_light - captured_dark < MIN_THRESHOLD_GAP) {
      ESP_LOGE(TAG,
               "rejected dark=%d light=%d: light must exceed dark by at least "
               "%d; keeping dark=%d light=%d",
               captured_dark, captured_light, MIN_THRESHOLD_GAP, threshold_dark,
               threshold_light);
      blink(5);
    } else {
      derive_thresholds(captured_dark, captured_light);
      if (thresholds_save(captured_dark, captured_light)) {
        ESP_LOGW(TAG,
                 "saved captured dark=%d light=%d -> thresholds dark=%d "
                 "light=%d",
                 captured_dark, captured_light, threshold_dark,
                 threshold_light);
      }
      blink(3);
    }
    state = STATE_NORMAL;
    break;

  default:
    ESP_LOGI(TAG, "short press ignored; hold BOOT for %d ms to calibrate",
             LONG_PRESS_MS);
    break;
  }
}

void calibration_init(void (*key_setter)(bool on)) {
  set_key = key_setter;

  storage_init();
  thresholds_load();
  button_init();

  button_events = xQueueCreate(BUTTON_QUEUE_LENGTH, sizeof(button_event_t));
  ESP_ERROR_CHECK(button_events == NULL ? ESP_ERR_NO_MEM : ESP_OK);
  xTaskCreate(button_task, "button", 2048, NULL, 10, NULL);

  ESP_LOGW(TAG, "ready: hold BOOT for %d ms to start calibration",
           LONG_PRESS_MS);
}

bool calibration_handle_input(int timeout_ms) {
  button_event_t event;
  if (xQueueReceive(button_events, &event, pdMS_TO_TICKS(timeout_ms)) !=
      pdTRUE) {
    return false;
  }

  if (event == BUTTON_LONG_PRESS) {
    handle_long_press();
  } else {
    handle_short_press();
  }
  return true;
}

bool calibration_active(void) { return state != STATE_NORMAL; }

const char *calibration_state_name(void) { return name_of(state); }

int calibration_threshold_dark(void) { return threshold_dark; }

int calibration_threshold_light(void) { return threshold_light; }
