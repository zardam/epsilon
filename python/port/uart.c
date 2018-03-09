#include "uart.h"

STATIC MP_DEFINE_CONST_FUN_OBJ_1(writeLine_obj, writeLine);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(readLine_obj, readLine);

STATIC const mp_rom_map_elem_t uart_module_globals_table[] = {
  { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_uart) },
  { MP_ROM_QSTR(MP_QSTR_writeLine), (mp_obj_t)&writeLine_obj },
  { MP_ROM_QSTR(MP_QSTR_readLine), (mp_obj_t)&readLine_obj },
};

STATIC MP_DEFINE_CONST_DICT(uart_module_globals, uart_module_globals_table);

const mp_obj_module_t uart_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&uart_module_globals,
};
