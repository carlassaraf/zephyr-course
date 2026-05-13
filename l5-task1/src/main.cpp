#include <zephyr/drivers/gpio.h>
#include <zephyr/kernel.h>
#include <zephyr/usb/usb_device.h>

/* The devicetree node identifier for the "ledr" alias. */
#define LEDR_NODE DT_ALIAS(ledr)
#define LEDG_NODE DT_ALIAS(ledg)
#define LEDB_NODE DT_ALIAS(ledb)

static const struct gpio_dt_spec ledr = GPIO_DT_SPEC_GET(LEDR_NODE, gpios);
static const struct gpio_dt_spec ledg = GPIO_DT_SPEC_GET(LEDG_NODE, gpios);
static const struct gpio_dt_spec ledb = GPIO_DT_SPEC_GET(LEDB_NODE, gpios);

int main(void)
{
    usb_enable(NULL);
    bool led_state = true;

    if (!gpio_is_ready_dt(&ledr)) return 0;

    if (gpio_pin_configure_dt(&ledr, GPIO_OUTPUT_ACTIVE) < 0) return 0;
    if (gpio_pin_configure_dt(&ledg, GPIO_OUTPUT_ACTIVE) < 0) return 0;
    if (gpio_pin_configure_dt(&ledb, GPIO_OUTPUT_ACTIVE) < 0) return 0;

    uint8_t i = 0;

    while (1) {

        if(i % 2 == 0) {
            if (gpio_pin_toggle_dt(&ledr) < 0) return 0;
        }
        if(i % 4 == 0) {
            if (gpio_pin_toggle_dt(&ledg) < 0) return 0;
        }
        if(i % 8 == 0) {
            if (gpio_pin_toggle_dt(&ledb) < 0) return 0;
        }

        i++;
        k_msleep(CONFIG_BLINKY_SLEEP_MS);
    }
    return 0;
}
