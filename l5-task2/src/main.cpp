#include <zephyr/kernel.h>
#include <zephyr/init.h>
#include <zephyr/logging/log.h>
#include <zephyr/usb/usb_device.h>

LOG_MODULE_REGISTER(app, LOG_LEVEL_INF);

// Basic application entry point
int main(void) {
    LOG_INF("Application Started");
    while(1) {
        k_sleep(K_SECONDS(1));
    }
    return 0;
}

// Callback function to be called before the application starts
static int pre_app_cb(void) {
    usb_enable(NULL);
    LOG_INF("Board Initialized");
    return 0;
}

// Initialize callback before the application starts
SYS_INIT(pre_app_cb, APPLICATION, 0);