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
  GPIOC.AFR()->setAlternateFunction(8, GPIO::AFR::AlternateFunction::AF9);
  GPIOC.AFR()->setAlternateFunction(9, GPIO::AFR::AlternateFunction::AF9);
  GPIOD.AFR()->setAlternateFunction(12, GPIO::AFR::AlternateFunction::AF9);
  GPIOD.AFR()->setAlternateFunction(13, GPIO::AFR::AlternateFunction::AF9);
}

void initQSPI() {
  QSPI.CR()->setPRESCALER(11); // 8MHz QSPI frequency
  QSPI.DCR()->setFSIZE(23);  // Number of bytes in Flash memory = 2^[FSIZE+1] (16 MBytes)
  QSPI.CCR()->setFMODE(3); // Memory mapped mode
  QSPI.CCR()->setDMODE(3); // Quad data lines
  QSPI.CCR()->setDCYC(8); // 1 dummy byte
  QSPI.CCR()->setADSIZE(2); // 24 bits address
  QSPI.CCR()->setADMODE(1); // Address on a single line
  QSPI.CCR()->setIMODE(1); // Instruction on a single line
  QSPI.CCR()->setINSTRUCTION(0x6B); // "Quad Output Fast Read" from the flash
  QSPI.CR()->setEN(true); // Enable QSPI Controller
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
