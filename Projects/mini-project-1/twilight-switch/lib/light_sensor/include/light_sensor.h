#ifndef LIGHT_SENSOR_H
#define LIGHT_SENSOR_H

void light_sensor_init(void);

/**
 * @brief Averaged photoresistor reading, 0..4095
 */
int light_sensor_read(void);

int light_sensor_to_millivolts(int raw);

#endif // LIGHT_SENSOR_H
