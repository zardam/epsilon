#ifndef ION_DEVICE_N0101_CONFIG_TIMING_H
#define ION_DEVICE_N0101_CONFIG_TIMING_H

#include <regs/regs.h>

namespace Ion {
namespace Device {
namespace Timing {
namespace Config {

constexpr static int LoopsPerMillisecond = 4426;
constexpr static int LoopsPerMicrosecond = 36;
// CPU clock is 96 MHz, and systick clock source is divided by 8
// To get 1 ms systick overflow we need to reset it to
// 96 000 000 (Hz) / 8 / 1 000 (ms/s) - 1 (because the counter resets *after* counting to 0)
// TODO: calibrate systick
constexpr static int SysTickPerMillisecond = 12000;


}
}
}
}

#endif