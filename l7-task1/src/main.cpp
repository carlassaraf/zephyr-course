#include <zephyr/kernel.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/shell/shell.h>

static const struct device *ledr = DEVICE_DT_GET(DT_ALIAS(ledr));
static const struct device *ledg = DEVICE_DT_GET(DT_ALIAS(ledg));
static const struct device *ledb = DEVICE_DT_GET(DT_ALIAS(ledb));

int main(void) {
    if (!device_is_ready(ledr) || !device_is_ready(ledg) || !device_is_ready(ledb)) {
        return 1;
    }
    while (1) {
        k_msleep(1);
    }
    return 0;
}

// Shell command handlers

// Root command
static int cmd_sensor(const struct shell *sh, size_t argc, char **argv) {
    shell_print(sh, "Use `sensor fetch`, `sensor read` or `sensor info`");
    return 0;
}

// Subcommand
static int cmd_sensor_fetch(const struct shell *sh, size_t argc, char **argv) {
    int ret = sensor_sample_fetch(ledr);
    if (ret) {
        shell_error(sh, "fetch failed: %d", ret);
        return ret;
    }
    shell_print(sh, "ledr: LED ON");
    return 0;
}

// Subcommand
static int cmd_sensor_read(const struct shell *sh, size_t argc, char **argv) {
    struct sensor_value val;
    int ret = sensor_channel_get(ledr, SENSOR_CHAN_ALL, &val);
    if (ret) {
        shell_error(sh, "read failed: %d", ret);
        return ret;
    }
    shell_print(sh, "ledr: LED OFF");
    return 0;
}

// Subcommand
static int cmd_sensor_info(const struct shell *sh, size_t argc, char **argv) {
    shell_print(sh, "ledr: %s", device_is_ready(ledr) ? "ready" : "not ready");
    return 0;
}

// Command registration

SHELL_STATIC_SUBCMD_SET_CREATE(sub_sensor,
    SHELL_CMD_ARG(fetch, NULL, "Turn LED on",  cmd_sensor_fetch, 1, 0),
    SHELL_CMD_ARG(read,  NULL, "Turn LED off", cmd_sensor_read,  1, 0),
    SHELL_CMD(info, NULL, "Show LED device info", cmd_sensor_info),
    SHELL_SUBCMD_SET_END
);

SHELL_CMD_REGISTER(sensor, &sub_sensor, "LED sensor commands", cmd_sensor);
