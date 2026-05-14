#include <zephyr/kernel.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/logging/log.h>
#include <zephyr/usb/usb_device.h>

#include <custom_led/custom_led.h>

LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);

int main(void) {
    usb_enable(NULL);
    const struct device *ledr = DEVICE_DT_GET(DT_ALIAS(ledr));
    const struct device *ledg = DEVICE_DT_GET(DT_ALIAS(ledg));
    const struct device *ledb = DEVICE_DT_GET(DT_ALIAS(ledb));

    if (!device_is_ready(ledr) || !device_is_ready(ledg) || !device_is_ready(ledb)) {
        LOG_ERR("Custom LED device not ready");
        return 0;
    }

    custom_led_set_name(ledr, "red");
    custom_led_set_name(ledg, "green");
    custom_led_set_name(ledb, "blue");
    LOG_INF("LED names set via custom extension API");

    // Log them
    char name[32] = "";
    custom_led_get_name(ledr, name);
    LOG_INF("ledr name is %s", name);

    custom_led_get_name(ledg, name);
    LOG_INF("ledg name is %s", name);

    custom_led_get_name(ledb, name);
    LOG_INF("ledb name is %s", name);

    struct sensor_value val;
    while (1) {
        sensor_sample_fetch(ledb);
        k_msleep(CONFIG_BLINKY_SLEEP_MS);
        sensor_channel_get(ledb, SENSOR_CHAN_ALL, &val);
        k_msleep(CONFIG_BLINKY_SLEEP_MS);
    }
    return 0;
}