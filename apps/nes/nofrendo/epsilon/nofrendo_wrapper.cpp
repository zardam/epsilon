#include <ion.h>
#include <kandinsky.h>
#include "nofrendo_wrapper.h"
#ifdef PLATFORM_DEVICE
#include "../../ion/src/device/display.h"
#endif

extern "C" {
  #include <nes.h>
  #include <event.h>
  #include <osd.h>
  #include <gui.h>
  #include "../bitmap.h"
}

NofrendoWrapper::NofrendoWrapper()
{
}

void NofrendoWrapper::run() {
  Ion::Display::pushRectUniform(KDRect(0, 0, Ion::Display::Width, Ion::Display::Height), KDColorBlack);
  nofrendo_main(0, NULL);
}

void draw_scanline(uint8_t *bmp, uint16_t *palette, int scanline) {
  if(scanline < 0 || scanline >= Ion::Display::Height) {
    return;
  }
  int xoffset = (Ion::Display::Width - NES_SCREEN_WIDTH) / 2;
  int yoffset = (Ion::Display::Height - NES_SCREEN_HEIGHT) / 2;
#if PLATFORM_DEVICE
  Ion::Display::Device::setDrawingArea(KDRect(xoffset, scanline+yoffset, NES_SCREEN_WIDTH, 1), Ion::Display::Device::Orientation::Portrait);
  *Ion::Display::Device::CommandAddress  = Ion::Display::Device::Command::MemoryWrite;
  uint8_t *pixels = bmp + NES_SCREEN_WIDTH + 7;
  size_t numberOfPixels = NES_SCREEN_WIDTH;
  while (numberOfPixels >= 32) {
    *Ion::Display::Device::DataAddress = palette[*pixels--];
    *Ion::Display::Device::DataAddress = palette[*pixels--];
    *Ion::Display::Device::DataAddress = palette[*pixels--];
    *Ion::Display::Device::DataAddress = palette[*pixels--];
    *Ion::Display::Device::DataAddress = palette[*pixels--];
    *Ion::Display::Device::DataAddress = palette[*pixels--];
    *Ion::Display::Device::DataAddress = palette[*pixels--];
    *Ion::Display::Device::DataAddress = palette[*pixels--];
    *Ion::Display::Device::DataAddress = palette[*pixels--];
    *Ion::Display::Device::DataAddress = palette[*pixels--];
    *Ion::Display::Device::DataAddress = palette[*pixels--];
    *Ion::Display::Device::DataAddress = palette[*pixels--];
    *Ion::Display::Device::DataAddress = palette[*pixels--];
    *Ion::Display::Device::DataAddress = palette[*pixels--];
    *Ion::Display::Device::DataAddress = palette[*pixels--];
    *Ion::Display::Device::DataAddress = palette[*pixels--];
    *Ion::Display::Device::DataAddress = palette[*pixels--];
    *Ion::Display::Device::DataAddress = palette[*pixels--];
    *Ion::Display::Device::DataAddress = palette[*pixels--];
    *Ion::Display::Device::DataAddress = palette[*pixels--];
    *Ion::Display::Device::DataAddress = palette[*pixels--];
    *Ion::Display::Device::DataAddress = palette[*pixels--];
    *Ion::Display::Device::DataAddress = palette[*pixels--];
    *Ion::Display::Device::DataAddress = palette[*pixels--];
    *Ion::Display::Device::DataAddress = palette[*pixels--];
    *Ion::Display::Device::DataAddress = palette[*pixels--];
    *Ion::Display::Device::DataAddress = palette[*pixels--];
    *Ion::Display::Device::DataAddress = palette[*pixels--];
    *Ion::Display::Device::DataAddress = palette[*pixels--];
    *Ion::Display::Device::DataAddress = palette[*pixels--];
    *Ion::Display::Device::DataAddress = palette[*pixels--];
    *Ion::Display::Device::DataAddress = palette[*pixels--];
    numberOfPixels -= 32;
  }

#else
  for(int x=0; x<NES_SCREEN_WIDTH+16; x++) {
    Ion::Display::pushRectUniform(
      KDRect(x+xoffset, scanline+yoffset, 1, 1),
      KDColor::RGB16(palette[bmp[x]])
    );
  }
#endif
}


void draw(bitmap_t *bmp, uint16 palette[]) {
  int xoffset = (Ion::Display::Width - bmp->width) / 2;
  int yoffset = (Ion::Display::Height - bmp->height) / 2;
#if PLATFORM_DEVICE
    for(int y=0; y<bmp->height; y++) {
      Ion::Display::Device::setDrawingArea(KDRect(xoffset, y+yoffset, bmp->width, 1), Ion::Display::Device::Orientation::Portrait);

      *Ion::Display::Device::CommandAddress  = Ion::Display::Device::Command::MemoryWrite;
      uint8_t *pixels = bmp->line[y] + bmp->width;
      size_t numberOfPixels = bmp->width;
      while (numberOfPixels >= 32) {
        *Ion::Display::Device::DataAddress = palette[*pixels--];
        *Ion::Display::Device::DataAddress = palette[*pixels--];
        *Ion::Display::Device::DataAddress = palette[*pixels--];
        *Ion::Display::Device::DataAddress = palette[*pixels--];
        *Ion::Display::Device::DataAddress = palette[*pixels--];
        *Ion::Display::Device::DataAddress = palette[*pixels--];
        *Ion::Display::Device::DataAddress = palette[*pixels--];
        *Ion::Display::Device::DataAddress = palette[*pixels--];
        *Ion::Display::Device::DataAddress = palette[*pixels--];
        *Ion::Display::Device::DataAddress = palette[*pixels--];
        *Ion::Display::Device::DataAddress = palette[*pixels--];
        *Ion::Display::Device::DataAddress = palette[*pixels--];
        *Ion::Display::Device::DataAddress = palette[*pixels--];
        *Ion::Display::Device::DataAddress = palette[*pixels--];
        *Ion::Display::Device::DataAddress = palette[*pixels--];
        *Ion::Display::Device::DataAddress = palette[*pixels--];
        *Ion::Display::Device::DataAddress = palette[*pixels--];
        *Ion::Display::Device::DataAddress = palette[*pixels--];
        *Ion::Display::Device::DataAddress = palette[*pixels--];
        *Ion::Display::Device::DataAddress = palette[*pixels--];
        *Ion::Display::Device::DataAddress = palette[*pixels--];
        *Ion::Display::Device::DataAddress = palette[*pixels--];
        *Ion::Display::Device::DataAddress = palette[*pixels--];
        *Ion::Display::Device::DataAddress = palette[*pixels--];
        *Ion::Display::Device::DataAddress = palette[*pixels--];
        *Ion::Display::Device::DataAddress = palette[*pixels--];
        *Ion::Display::Device::DataAddress = palette[*pixels--];
        *Ion::Display::Device::DataAddress = palette[*pixels--];
        *Ion::Display::Device::DataAddress = palette[*pixels--];
        *Ion::Display::Device::DataAddress = palette[*pixels--];
        *Ion::Display::Device::DataAddress = palette[*pixels--];
        *Ion::Display::Device::DataAddress = palette[*pixels--];
        numberOfPixels -= 32;
      }
      while (numberOfPixels--) {
        *Ion::Display::Device::DataAddress = palette[*pixels--];
      }
    }
#else
    for(int x=0; x<bmp->width; x++) {
      for(int y=0; y<bmp->height; y++) {
        Ion::Display::pushRectUniform(
          KDRect(x+xoffset, y+yoffset, 1, 1),
          KDColor::RGB16(palette[bmp->line[y][x]])
        );
      }
    }
#endif
}

uint64_t readKeyboard() {
  return Ion::Keyboard::scan();
}

int nofrendo_ticks() {
  return Ion::millis() / 20;
}
