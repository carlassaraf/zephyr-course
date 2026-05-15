#include <zephyr/kernel.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/shell/shell.h>
#include <custom_led/custom_led.h>

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
    shell_info(sh, "Use `sensor fetch`, `sensor read` or `sensor info`");
    return 0;
}

// Subcommand
static int cmd_sensor_fetch(const struct shell *sh, size_t argc, char **argv) {
    int ret = sensor_sample_fetch(ledr);
    if (ret) {
        shell_error(sh, "fetch failed: %d", ret);
        return ret;
    }
    shell_info(sh, "ledr: LED ON");
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
    shell_info(sh, "ledr: LED OFF");
    return 0;
}

// Subcommand
static int cmd_sensor_info(const struct shell *sh, size_t argc, char **argv) {
    shell_info(sh, "ledr: %s", device_is_ready(ledr) ? "ready" : "not ready");
    return 0;
}

// Subcommand
static int cmd_sensor_set_name(const struct shell *sh, size_t argc, char **argv) {
    if(argc != 3) {
        shell_error(sh, "Wrong number of arguments. Usage: sensor set <led> <name>");
        return -1;
    } 
    int led = atoi(argv[1]);
    if(led < 0 || led > 2) {
        shell_error(sh, "Out of range. Available: 0-2");
        return -1;
    }

    if(led == 0) { 
        custom_led_set_name(ledr, argv[2]);
    } else if(led == 1) {
        custom_led_set_name(ledg, argv[2]);
    } else if(led == 2) {
        custom_led_set_name(ledb, argv[2]);
    }
    shell_info(sh, "LED%d name set", led);
    return 0;
}

// Subcommand
static int cmd_sensor_get_name(const struct shell *sh, size_t argc, char **argv) {
    if(argc != 2) {
        shell_error(sh, "Wrong number of arguments. Usage: sensor get <led>");
        return -1;
    } 
    int led = atoi(argv[1]);
    if(led < 0 || led > 2) {
        shell_error(sh, "Out of range. Available: 0-2");
        return -1;
    }

    char name[32];
    if(led == 0) { 
        custom_led_get_name(ledr, name);
    } else if(led == 1) {
        custom_led_set_name(ledg, name);
    } else if(led == 2) {
        custom_led_set_name(ledb, name);
    }
    shell_info(sh, "LED%d name is %s", led, name);
    return 0;
}

// Command registration

SHELL_STATIC_SUBCMD_SET_CREATE(sub_sensor,
    SHELL_CMD_ARG(fetch, NULL, "Turn LED on",  cmd_sensor_fetch, 1, 0),
    SHELL_CMD_ARG(read,  NULL, "Turn LED off", cmd_sensor_read,  1, 0),
    SHELL_CMD_ARG(set, NULL, "Set LED name", cmd_sensor_set_name, 3, 0),
    SHELL_CMD_ARG(get, NULL, "Get LED name", cmd_sensor_get_name, 2, 0),
    SHELL_CMD(info, NULL, "Show LED device info", cmd_sensor_info),
    SHELL_SUBCMD_SET_END
);

SHELL_CMD_REGISTER(sensor, &sub_sensor, "LED sensor commands", cmd_sensor);
