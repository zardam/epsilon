#include "helpers.h"
#include <ion.h>
#ifdef SIMULATOR
#include <FL/Fl.H>
#endif
extern "C" {
#include "mphalport.h"
}
#include <iostream>

bool micropython_port_vm_hook_loop() {
  /* This function is called very frequently by the MicroPython engine. We grab
   * this opportunity to interrupt execution and/or refresh the display on
   * platforms that need it. */

  /* Doing too many things here slows down Python execution quite a lot. So we
   * only do things once in a while and return as soon as possible otherwise. */
  static int c = 0;

  c = (c + 1) % 20000;
  if (c != 0) {
    return false;
  }

  // Check if the user asked for an interruption from the keyboard
  return micropython_port_interrupt_if_needed();
}

bool micropython_port_interruptible_msleep(uint32_t delay) {
  uint32_t start = Ion::Timing::millis();
  while (Ion::Timing::millis() - start < delay) {
    if (micropython_port_interrupt_if_needed()) {
      return true;
    }
    Ion::Timing::msleep(1);
  }
  return false;
}

bool micropython_port_interrupt_if_needed() {
  Ion::Keyboard::State scan = Ion::Keyboard::scan();
  Ion::Keyboard::Key interruptKey = static_cast<Ion::Keyboard::Key>(mp_interrupt_char);
  if (scan.keyDown(interruptKey)) {
    mp_keyboard_interrupt();
    return true;
  }
  return false;
}

int micropython_port_random() {
  return Ion::random();
}

bool back_key_pressed() {
  static int c = 0;

  ++c ;
  if (c<400 || (c & 0xf)!= 0) {
    return false;
  }
#ifdef SIMULATOR
  Fl::wait(0.00001);
#endif

  Ion::Keyboard::State scan = Ion::Keyboard::scan();
  // if (scan!=16) std::cerr << scan << '\n';
  Ion::Keyboard::Key interruptKey = static_cast<Ion::Keyboard::Key>(mp_interrupt_char);
  if (scan.keyDown(interruptKey)
      // || scan.keyDown(static_cast<Ion::Keyboard::Key>(16))
      ) 
    return true;
  return false;
}
