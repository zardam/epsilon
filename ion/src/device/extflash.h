#ifndef ION_DEVICE_EXTFLASH_H
#define ION_DEVICE_EXTFLASH_H

extern "C" {
#include <stdint.h>
}

namespace Ion {
namespace ExtFlash {
namespace Device {

/*  Pin | Role              | Mode
 * -----+-------------------+-----------------------
 *  PB2 | QSPI CLOCK        | Alternate Function 9
 *  PB6 | QSPI NCS          | Alternate Function 10
 *  PC8 | QSPI IO2          | Alternate Function 10
 *  PC9 | QSPI IO0          | Alternate Function 9
 * PD12 | QSPI IO1          | Alternate Function 9
 * PD13 | QSPI IO3          | Alternate Function 9
 */

void init();
void shutdown();

}
}
}

#endif
