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
  
  // Set GPIOs Speed
  GPIOB.OSPEEDR()->setSpeed(2, GPIO::OSPEEDR::Speed::High);
  GPIOB.OSPEEDR()->setSpeed(6, GPIO::OSPEEDR::Speed::High);
  GPIOC.OSPEEDR()->setSpeed(8, GPIO::OSPEEDR::Speed::High);
  GPIOC.OSPEEDR()->setSpeed(9, GPIO::OSPEEDR::Speed::High);
  GPIOD.OSPEEDR()->setSpeed(12, GPIO::OSPEEDR::Speed::High);
  GPIOD.OSPEEDR()->setSpeed(13, GPIO::OSPEEDR::Speed::High);
}

void initQSPI() {
  QSPI.CR()->setPRESCALER(0); // 96MHz QSPI frequency
  QSPI.DCR()->setFSIZE(22); // Number of bytes in Flash memory = 2^[FSIZE+1] (8 MBytes)
  QSPI.CCR()->setSIOO(true); // Send instruction only once mode
  QSPI.CCR()->setFMODE(3); // Memory mapped mode
  QSPI.CCR()->setDMODE(3); // Quad data lines
  QSPI.CCR()->setDCYC(4); // 4 dummy cycles
  QSPI.CCR()->setABSIZE(0); // One alternate byte
  QSPI.CCR()->setABMODE(3); // Alternate byte on four lines
  QSPI.CCR()->setADSIZE(2); // 24 bits address
  QSPI.CCR()->setADMODE(3); // Address on four lines
  QSPI.CCR()->setIMODE(1); // Instruction on a single line
  QSPI.CCR()->setINSTRUCTION(0xeb); // "Fast Read Quad I/O" from the flash
  QSPI.ABR()->setALTERNATE(0xa0); // Send instruction only once
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
