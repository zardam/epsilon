#ifndef ION_DEVICE_N0101_CONFIG_BACKLIGHT_H
#define ION_DEVICE_N0101_CONFIG_BACKLIGHT_H

#include <regs/regs.h>

/*  Pin | Role              | Mode                  | Function
 * -----+-------------------+-----------------------+----------
 *  PE0 | Backlight Enable  | Output                |
 */

namespace Ion {
namespace Device {
namespace Backlight {
namespace Config {

constexpr static GPIOPin BacklightPin = GPIOPin(GPIOE, 0);

}
}
}
}

#endif