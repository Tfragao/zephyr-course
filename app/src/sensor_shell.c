#include <zephyr/kernel.h>
#include <zephyr/shell/shell.h>
#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <our_drivers/our_driver.h>
#include <stdlib.h>
#include <errno.h>

static const struct device *sensor_dev =
    DEVICE_DT_GET_ANY(our_driver);

/* sensor fetch */
static int cmd_sensor_fetch(const struct shell *sh,
                            size_t argc,
                            char **argv)
{
    ARG_UNUSED(argc);
    ARG_UNUSED(argv);

    if (!device_is_ready(sensor_dev)) {
        shell_error(sh, "Sensor device is not ready");
        return -ENODEV;
    }

    int ret = sensor_sample_fetch(sensor_dev);

    if (ret < 0) {
        shell_error(sh, "sensor_sample_fetch() failed: %d", ret);
        return ret;
    }

    shell_print(sh, "sensor_sample_fetch(): success");

    return 0;
}

/* sensor read */
static int cmd_sensor_read(const struct shell *sh,
                           size_t argc,
                           char **argv)
{
    ARG_UNUSED(argc);
    ARG_UNUSED(argv);

    if (!device_is_ready(sensor_dev)) {
        shell_error(sh, "Sensor device is not ready");
        return -ENODEV;
    }

    struct sensor_value value;

    int ret = sensor_channel_get(sensor_dev,
                                 SENSOR_CHAN_LIGHT,
                                 &value);

    if (ret < 0) {
        shell_error(sh, "sensor_channel_get() failed: %d", ret);
        return ret;
    }

    shell_print(sh, "Sensor value: %d", value.val1);

    return 0;
}

/* sensor info */
static int cmd_sensor_info(const struct shell *sh,
                           size_t argc,
                           char **argv)
{
    ARG_UNUSED(argc);
    ARG_UNUSED(argv);

    shell_print(sh,
                "Device: %s",
                sensor_dev->name);

    shell_print(sh,
                "Ready: %s",
                device_is_ready(sensor_dev) ? "yes" : "no");

    return 0;
}

static int cmd_sensor_set(const struct shell *sh,
                          size_t argc,
                          char **argv)
{
    ARG_UNUSED(argc);

    if (!device_is_ready(sensor_dev)) {
        shell_error(sh, "Sensor device is not ready");
        return -ENODEV;
    }

    char *endptr;
    long value = strtol(argv[1], &endptr, 10);

    if (*endptr != '\0') {
        shell_error(sh, "Invalid value: %s", argv[1]);
        return -EINVAL;
    }

    if (value < 0 || value > 100) {
        shell_error(sh, "Value must be between 0 and 100");
        return -ERANGE;
    }

    int ret = our_driver_set_custom_value(sensor_dev,
                                           (int)value);

    if (ret < 0) {
        shell_error(sh,
                    "Failed to set value: %d",
                    ret);
        return ret;
    }

    shell_print(sh,
                "Custom value set to %ld",
                value);

    return 0;
}

SHELL_STATIC_SUBCMD_SET_CREATE(
    sensor_cmds,

    SHELL_CMD(
        fetch,
        NULL,
        "Call sensor_sample_fetch()",
        cmd_sensor_fetch
    ),

    SHELL_CMD(
        read,
        NULL,
        "Call sensor_channel_get() and print the result",
        cmd_sensor_read
    ),

    SHELL_CMD(
        info,
        NULL,
        "Show sensor device information",
        cmd_sensor_info
    ),

    SHELL_CMD_ARG(
        set,
        NULL,
        "Set custom value: sensor set <value>",
        cmd_sensor_set,
        2,
        2
    ),

    SHELL_SUBCMD_SET_END
);

SHELL_CMD_REGISTER(
    sensor,
    &sensor_cmds,
    "Sensor commands",
    NULL
);
