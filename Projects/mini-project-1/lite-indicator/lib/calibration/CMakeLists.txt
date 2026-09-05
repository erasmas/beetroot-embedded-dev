idf_component_register(SRCS "calibration.c"
                       INCLUDE_DIRS "include"
                       PRIV_REQUIRES esp_driver_gpio esp_timer nvs_flash
                                     light_sensor)
