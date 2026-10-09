#ifndef OUR_DRIVER_H_
#define OUR_DRIVER_H_

#include <zephyr/device.h>

int our_driver_set_custom_value(const struct device *dev, int value);

#endif
