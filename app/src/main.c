#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <our_drivers/our_driver.h>

int main(void)
{
    const struct device *dev = DEVICE_DT_GET_ANY(our_driver);

    if (!device_is_ready(dev)) {
        printk("Our driver is not ready\n");
        return 0;
    }

    printk("Our LED sensor driver is ready\n");
    
    int ret = our_driver_set_custom_value(dev, 77);

    if (ret < 0) {
          printk("Failed to set custom value: %d\n", ret);
	} else {
    	   printk("Custom driver value set to 42\n");
        }

    while (1) {
        struct sensor_value value;

        printk("sensor_sample_fetch(): LED ON\n");

        int ret = sensor_sample_fetch(dev);
        if (ret < 0) {
            printk("sample_fetch failed: %d\n", ret);
        }

        k_sleep(K_MSEC(500));

        printk("sensor_channel_get(): LED OFF\n");

        ret = sensor_channel_get(dev, SENSOR_CHAN_LIGHT, &value);
        if (ret < 0) {
            printk("channel_get failed: %d\n", ret);
        } else {
            printk("LED state was: %d\n", value.val1);
        }

        k_sleep(K_MSEC(500));
    }

    return 0;
}
