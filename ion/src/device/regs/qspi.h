#ifndef QSPI_H
#define QSPI_H

#include "register.h"

class QSPI {
public:
  class CR : public Register32 {
  public:
    REGS_FIELD(PRESCALER, uint8_t, 31, 24);
    REGS_BOOL_FIELD(EN, 0);
  };
  class DCR : public Register32 {
  public:
    REGS_FIELD(FSIZE, uint8_t, 20, 16);
    REGS_BOOL_FIELD(CKMODE, 0);
  };

  constexpr QSPI() {};
  REGS_REGISTER_AT(CR, 0x00);
  REGS_REGISTER_AT(DCR, 0x04);
private:
  constexpr uint32_t Base() const {
    return 0xA0001000;
  }
};

constexpr QSPI QSPI;

#endif
