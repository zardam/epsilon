extern "C" {
#include "uart.h"
}
#include <ion.h>
#include "port.h"

char line[64];

mp_obj_t writeLine(mp_obj_t a) {
  Ion::Console::writeLine(mp_obj_str_get_str(a));
  return mp_const_none;
}

mp_obj_t readLine() {
  Ion::Console::readLine(line, 64);
  return mp_obj_new_str(line, strlen(line), false);
}
