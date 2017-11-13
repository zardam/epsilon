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
  class SR : public Register32 {
  public:
    REGS_BOOL_FIELD_R(BUSY, 5);
  };
  class CCR : public Register32 {
  public:
    REGS_BOOL_FIELD(SIOO, 28);
    REGS_FIELD(FMODE, uint8_t, 27, 26);
    REGS_FIELD(DMODE, uint8_t, 25, 24);
    REGS_FIELD(DCYC, uint8_t, 22, 18);
    REGS_FIELD(ABSIZE, uint8_t, 17, 16);
    REGS_FIELD(ABMODE, uint8_t, 15, 14);
    REGS_FIELD(ADSIZE, uint8_t, 13, 12);
    REGS_FIELD(ADMODE, uint8_t, 11, 10);
    REGS_FIELD(IMODE, uint8_t, 9, 8);
    REGS_FIELD(INSTRUCTION, uint8_t, 7, 0);
  };

  class ABR : public Register32 {
  public:
    REGS_FIELD(ALTERNATE, uint32_t, 31, 0);
  };

  constexpr QSPI() {};
  REGS_REGISTER_AT(CR, 0x00);
  REGS_REGISTER_AT(DCR, 0x04);
  REGS_REGISTER_AT(SR, 0x08);
  REGS_REGISTER_AT(CCR, 0x14);
  REGS_REGISTER_AT(ABR, 0x1c);
private:
  constexpr uint32_t Base() const {
    return 0xA0001000;
  }
};

constexpr QSPI QSPI;

#endif
