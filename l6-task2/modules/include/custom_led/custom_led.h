#ifndef DRIVER_CUSTOM_LED_H
#define DRIVER_CUSTOM_LED_H

#include <zephyr/drivers/sensor.h>

// Extended API types
typedef int (*custom_led_set_name_t)(const struct device *dev, const char *name);
typedef int (*custom_led_get_name_t)(const struct device *dev, char *name);

// Sensor API implementation + extended API
struct custom_led_driver_api {
  struct sensor_driver_api sensor;
  custom_led_set_name_t set_name;
  custom_led_get_name_t get_name;
};

// Expose custom implementations

static inline int custom_led_set_name(const struct device *dev, const char *name) {
  const struct custom_led_driver_api *api = (const struct custom_led_driver_api *)dev->api;
  return api->set_name(dev, name);
}

static inline int custom_led_get_name(const struct device *dev, char *name) {
  const struct custom_led_driver_api *api = (const struct custom_led_driver_api *)dev->api;
  return api->get_name(dev, name);
}

#endif