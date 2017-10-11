#include "extflash.h"
#include "regs/regs.h"
extern "C" {
#include <assert.h>
}

namespace Ion {
namespace ExtFlash {
namespace Device {

void initGPIO() {
  // Configure GPIOs to use AF
  GPIOB.MODER()->setMode(2, GPIO::MODER::Mode::AlternateFunction);
  GPIOB.MODER()->setMode(6, GPIO::MODER::Mode::AlternateFunction);
  GPIOC.MODER()->setMode(8, GPIO::MODER::Mode::AlternateFunction);
  GPIOC.MODER()->setMode(9, GPIO::MODER::Mode::AlternateFunction);
  GPIOD.MODER()->setMode(12, GPIO::MODER::Mode::AlternateFunction);
  GPIOD.MODER()->setMode(13, GPIO::MODER::Mode::AlternateFunction);

  // Set GPIOs AF
  GPIOB.AFR()->setAlternateFunction(2, GPIO::AFR::AlternateFunction::AF9);
  GPIOB.AFR()->setAlternateFunction(6, GPIO::AFR::AlternateFunction::AF10);
  GPIOC.AFR()->setAlternateFunction(8, GPIO::AFR::AlternateFunction::AF10);
  GPIOC.AFR()->setAlternateFunction(9, GPIO::AFR::AlternateFunction::AF9);
  GPIOD.AFR()->setAlternateFunction(12, GPIO::AFR::AlternateFunction::AF9);
  GPIOD.AFR()->setAlternateFunction(13, GPIO::AFR::AlternateFunction::AF9);
}

void initQSPI() {
  // QSPI.CR()->setEN(true);
}

void init() {
  initGPIO();
  initQSPI();
}

void shutdown() {

}

}
}
}
