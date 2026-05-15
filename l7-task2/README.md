# L6 Task 1 — Custom Zephyr Driver Using an Existing Driver API

This task implements a custom GPIO LED driver using the Zephyr sensor driver API as a template.

- `sensor_sample_fetch` → toggles the LED on/off
- `sensor_channel_get` → reads the current LED state

---

## The Zephyr Driver Model

Every peripheral in Zephyr is represented as a `struct device`. That struct holds three pointers:

- `config` — read-only, set at compile time (from devicetree)
- `data` — read-write runtime state
- `api` — a table of function pointers

When your application calls `sensor_sample_fetch(dev)`, it dereferences that API table and calls the right function pointer. The driver fills in that table. The devicetree ties a driver to a physical peripheral at compile time.

---

## Step 1 — Pick an Existing Driver API to Implement

Zephyr has many built-in driver classes: `sensor`, `gpio`, `uart`, `i2c`, etc. Each class defines a struct of function pointers — the API contract. You implement those functions, point the struct at them, and your driver looks like any other driver of that class to the rest of the system.

This task reuses **`sensor_driver_api`** because it provides two hooks that map cleanly to toggle and read. No real sensor is needed — the abstraction is borrowed.

```c
// From zephyr/drivers/sensor.h — the contract you are implementing
struct sensor_driver_api {
    sensor_attr_set_t    attr_set;
    sensor_attr_get_t    attr_get;
    sensor_trigger_set_t trigger_set;
    sensor_sample_fetch_t sample_fetch;   // ← implement this
    sensor_channel_get_t  channel_get;    // ← and this
    ...
};
```

---

## Step 2 — Write the DTS Binding YAML

Before any C code, you must tell the **devicetree compiler** what a node with your compatible string looks like. This is the YAML binding file.

```yaml
# modules/dts/bindings/driver/custom_led.yaml
description: Custom LED sensor
compatible: "course,custom-led"
include: base.yaml
properties:
  gpios:
    type: phandle-array   # tells dtc this is a GPIO reference, not raw bytes
    required: true
```

The `type: phandle-array` is critical. Without it the DTS compiler treats `gpios` as opaque bytes and never generates the ordinal macros that `GPIO_DT_SPEC_INST_GET` depends on at compile time.

---

## Step 3 — Write the Driver Source

The driver source has five parts, always in this order.

### 3a. The Compat Macro

Links C code to the DTS node. Take the `compatible` string and replace `,` and `-` with `_`.

```c
#define DT_DRV_COMPAT course_custom_led   // from "course,custom-led"
```

### 3b. Config and Data Structs

`config` is compile-time constant (populated from devicetree). `data` holds runtime state.

```c
struct custom_led_config {
    struct gpio_dt_spec led_pin;  // filled by GPIO_DT_SPEC_INST_GET at compile time
};

struct custom_led_data {
    bool led_on;                  // changes at runtime
};
```

### 3c. The Driver Functions

```c
static int custom_led_init(const struct device *dev) {
    const struct custom_led_config *config = dev->config;
    if (!gpio_is_ready_dt(&config->led_pin)) { return -ENODEV; }
    return gpio_pin_configure_dt(&config->led_pin, GPIO_OUTPUT_INACTIVE);
}

static int custom_led_sample_fetch(const struct device *dev, enum sensor_channel chan) {
    struct custom_led_data *data = dev->data;
    const struct custom_led_config *cfg = dev->config;
    data->led_on = !data->led_on;
    return gpio_pin_set_dt(&cfg->led_pin, data->led_on);
}

static int custom_led_channel_get(const struct device *dev, enum sensor_channel chan,
                                   struct sensor_value *val) {
    struct custom_led_data *data = dev->data;
    val->val1 = data->led_on;
    val->val2 = 0;
    return 0;
}
```

### 3d. The API Table

Glues your functions to the class contract.

```c
static const struct sensor_driver_api custom_led_api = {
    .sample_fetch = custom_led_sample_fetch,
    .channel_get  = custom_led_channel_get,
};
```

### 3e. The Registration Macro

Creates the actual device. Without this, no `struct device` exists and `DEVICE_DT_GET` returns garbage.

```c
#define CUSTOM_LED_DEFINE(inst)                                          \
    static struct custom_led_data custom_led_data_##inst;                \
    static const struct custom_led_config custom_led_config_##inst = {   \
        .led_pin = GPIO_DT_SPEC_INST_GET(inst, gpios),                   \
    };                                                                    \
    SENSOR_DEVICE_DT_INST_DEFINE(inst, custom_led_init, NULL,            \
                                 &custom_led_data_##inst,                \
                                 &custom_led_config_##inst,              \
                                 POST_KERNEL,                            \
                                 CONFIG_SENSOR_INIT_PRIORITY,            \
                                 &custom_led_api);

DT_INST_FOREACH_STATUS_OKAY(CUSTOM_LED_DEFINE)
```

`DT_INST_FOREACH_STATUS_OKAY` iterates over every enabled DTS node with your compatible string and expands `CUSTOM_LED_DEFINE` once per node. `GPIO_DT_SPEC_INST_GET(inst, gpios)` reads the GPIO controller, pin number, and flags from the DTS node and populates `led_pin` — all at compile time.

---

## Step 4 — Set Up the Build System

There are three layers, each with a CMakeLists.txt and a Kconfig:

```
modules/
├── zephyr/module.yml              ← registers this folder as a Zephyr module
├── CMakeLists.txt                 ← add_subdirectory(drivers/custom_led)
├── Kconfig                        ← rsource "drivers/custom_led/Kconfig"
└── drivers/custom_led/
    ├── CMakeLists.txt             ← zephyr_library() + sources + include dirs
    └── Kconfig                    ← config CUSTOM_LED (depends on GPIO && SENSOR)
```

### module.yml

Tells Zephyr where to find the CMakeLists.txt, Kconfig, and DTS bindings for this module.

```yaml
name: custom_drivers
build:
  cmake: .
  kconfig: Kconfig
  dts:
    dirs:
      - dts/bindings    # Zephyr searches here for *.yaml binding files
```

### Driver CMakeLists.txt

Registers the driver as a compilable library.

```cmake
zephyr_library()
zephyr_library_sources(custom_led.c)
zephyr_library_include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../include)
```

### Driver Kconfig

Creates the toggle that lets `prj.conf` enable or disable the driver.

```kconfig
config CUSTOM_LED
    bool "Custom LED driver"
    depends on GPIO && SENSOR
```

### App CMakeLists.txt

Must register the module and the DTS root **before** `find_package`:

```cmake
list(APPEND ZEPHYR_EXTRA_MODULES ${CMAKE_CURRENT_LIST_DIR}/modules)
list(APPEND DTS_ROOT             ${CMAKE_CURRENT_LIST_DIR}/modules)
find_package(Zephyr REQUIRED)
```

`ZEPHYR_EXTRA_MODULES` wires up CMake and Kconfig. `DTS_ROOT` separately tells the DTS compiler to search `modules/dts/bindings/` for binding YAMLs. These are two independent paths in the build system — both are required.

---

## Step 5 — Create the DTS Overlay

The overlay instantiates your driver on actual hardware. Without a node with your compatible string, `DT_INST_FOREACH_STATUS_OKAY` expands to nothing and the driver is never created.

```dts
// app.overlay
/ {
    custom_led_0: custom-led {
        compatible = "course,custom-led";
        gpios = <&gpio0 25 GPIO_ACTIVE_HIGH>;   // adjust pin for your board
    };
};
```

The label `custom_led_0` is what you reference in the application with `DT_NODELABEL(custom_led_0)`.

---

## Step 6 — Update prj.conf

```
CONFIG_GPIO=y
CONFIG_SENSOR=y       # enables the sensor subsystem API you are implementing
CONFIG_CUSTOM_LED=y   # enables your driver via the Kconfig you wrote
```

---

## Step 7 — Use the Driver in the Application

```cpp
const struct device *dev = DEVICE_DT_GET(DT_NODELABEL(custom_led_0));

sensor_sample_fetch(dev);                        // calls custom_led_sample_fetch → toggles LED
sensor_channel_get(dev, SENSOR_CHAN_ALL, &val);  // calls custom_led_channel_get → reads state
```

`DEVICE_DT_GET` is a compile-time macro that resolves to the static device object created by `SENSOR_DEVICE_DT_INST_DEFINE`. There is no runtime lookup — the connection between app and driver is resolved at compile time.

---

## The Compile-Time Chain

```
app.overlay                        binding YAML              custom_led.c
-----------                        ------------              ------------
compatible = "course,custom-led"   ←→  DT_DRV_COMPAT course_custom_led
gpios = <&gpio0 25 HIGH>           type: phandle-array  ←→  GPIO_DT_SPEC_INST_GET(inst, gpios)
&custom_led_0                                           ←→  DEVICE_DT_GET(DT_NODELABEL(custom_led_0))
```

Everything is resolved by the preprocessor and the DTS compiler before any C is compiled. At runtime there is nothing dynamic — just a static device struct and a function pointer table.

---

## Common Pitfalls

| Mistake | Symptom |
|---|---|
| `DT_DRV_COMPAT` doesn't match the binding `compatible` | Driver instantiates zero devices silently |
| Missing `DTS_ROOT` in app CMakeLists.txt | `gpios` treated as raw bytes, ordinal macros undefined |
| `gpio_is_ready` instead of `gpio_is_ready_dt` | Compile error or wrong check |
| Config field name mismatch (`led` vs `led_pin`) | Compile error or wrong GPIO referenced |
| Stale build cache after adding `ZEPHYR_EXTRA_MODULES` | Module not picked up — run `west build --pristine` |
| Missing `SENSOR_DEVICE_DT_INST_DEFINE` | No device created, `DEVICE_DT_GET` is invalid |
