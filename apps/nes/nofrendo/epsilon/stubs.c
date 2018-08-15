#ifdef PLATFORM_DEVICE

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

int printf(const char* format, ...) {
  return 0;
}

int sprintf(char *str, const char *format, ...) {
  return 0;
}

char *strncat(char *dest, const char *src, size_t n) {
  return NULL;
}

char *strncpy(char *dest, const char *src, size_t n) {
  return NULL;
}

FILE *fopen(const char * filename, const char * mode) {
  return NULL;
}

size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream) {
  return 0;
}

int fclose(FILE *stream) {
  return 0;
}

int fputc(int c, FILE *stream) {
  return 0;
}

size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream) {
  return 0;
}

char *strcpy(char *dest, const char *src) {
  return NULL;
}

int fprintf(FILE *stream, const char *format, ...) {
  return 0;
}

int strcasecmp(const char *s1, const char *s2) {
  return 0;
}

int isspace(int c) {
  return 0;
}

int vsprintf(char *str, const char *format, va_list ap) {
  return 0;
}

unsigned long int strtoul(const char *nptr, char **endptr, int base) {
  return 0;
}

char *strrchr(const char *s, int c) {
  return 0;
}

int strncmp(const char *s1, const char *s2, size_t n) {
  return 0;
}

char *strcat(char *dest, const char *src) {
  return NULL;
}

char *fgets(char *s, int size, FILE *stream) {
  return NULL;
}

int feof(FILE *stream) {
  return 0;
}

int rand(void) {
  return 0;
}

int atexit(void (*function)(void)) {
  return 0;
}

#endif

// nofrendo
#include <gui.h>
#include <vid_drv.h>
#include "nofrendo_wrapper.h"

void gui_tick(int ticks) {}
void gui_setrefresh(int frequency) {}

void gui_sendmsg(int color, char *format, ...) {}

int gui_init(void) {
  return 0;
}
void gui_shutdown(void) {}

void gui_frame(bool draw) {}

void gui_togglefps(void) {}
void gui_togglegui(void) {}
void gui_togglewave(void) {}
void gui_togglepattern(void) {}
void gui_toggleoam(void) {}

void gui_decpatterncol(void) {}
void gui_incpatterncol(void) {}

void gui_savesnap(void) {}
void gui_togglesprites(void) {}
void gui_togglefs(void) {}
void gui_displayinfo() {}
void gui_toggle_chan(int chan) {}
void gui_setfilter(int filter_type) {}

rgb_t gui_pal[GUI_TOTALCOLORS] =
{
   { 0x00, 0x00, 0x00 }, /* black      */
   { 0x3F, 0x3F, 0x3F }, /* dark gray  */
   { 0x7F, 0x7F, 0x7F }, /* gray       */
   { 0xBF, 0xBF, 0xBF }, /* light gray */
   { 0xFF, 0xFF, 0xFF }, /* white      */
   { 0xFF, 0x00, 0x00 }, /* red        */
   { 0x00, 0xFF, 0x00 }, /* green      */
   { 0x00, 0x00, 0xFF }, /* blue       */
   { 0xFF, 0xFF, 0x00 }, /* yellow     */
   { 0xFF, 0xAF, 0x00 }, /* orange     */
   { 0xFF, 0x00, 0xFF }, /* purple     */
   { 0x3F, 0x7F, 0x7F }, /* teal       */
   { 0x00, 0x2A, 0x00 }, /* dk. green  */
   { 0x00, 0x00, 0x3F }  /* dark blue  */
};

static bitmap_t *primary_buffer = NULL;
static uint16 myPalette[256];

int  vid_init(int width, int height, viddriver_t *osd_driver) {
  return 0;
}
void vid_flush(void) {
  draw(primary_buffer, myPalette);
}

void vid_setpalette(rgb_t *pal) {
  uint16 c;

   int i;

   for (i = 0; i < 256; i++)
   {
      c=(pal[i].b>>3)+((pal[i].g>>2)<<5)+((pal[i].r>>3)<<11);
      //myPalette[i]=(c>>8)|((c&0xff)<<8);
      myPalette[i]=c;
   }

}
int  vid_setmode(int width, int height) {
  if (NULL != primary_buffer)
     bmp_destroy(&primary_buffer);

  primary_buffer = bmp_create(width, height, 0); /* no overdraw */
  if (NULL == primary_buffer)
     return -1;

  bmp_clear(primary_buffer, GUI_BLACK);

  return 0;
}

#define  DUFFS_DEVICE(transfer, count) \
{ \
   register int n = (count + 7) / 8; \
   switch (count % 8) \
   { \
   case 0:  do {  { transfer; } \
   case 7:        { transfer; } \
   case 6:        { transfer; } \
   case 5:        { transfer; } \
   case 4:        { transfer; } \
   case 3:        { transfer; } \
   case 2:        { transfer; } \
   case 1:        { transfer; } \
            } while (--n > 0); \
   } \
}

INLINE void vid_memcpy(void *dest, const void *src, int len)
{
   uint32 *s = (uint32 *) src;
   uint32 *d = (uint32 *) dest;

   ASSERT(0 == ((len & 3) | ((uint32) src & 3) | ((uint32) dest & 3)));
   len >>= 2;

   DUFFS_DEVICE(*d++ = *s++, len);
}

/* blits a bitmap onto primary buffer */
void vid_blit(bitmap_t *bitmap, int src_x, int src_y, int dest_x, int dest_y,
              int width, int height)
{
   int bitmap_pitch, primary_pitch;
   uint8 *dest_ptr, *src_ptr;

   ASSERT(bitmap);

   /* clip to source */
   if (src_y >= bitmap->height)
      return;
   if (src_y + height > bitmap->height)
      height = bitmap->height - src_y;

   if (src_x >= bitmap->width)
      return;
   if (src_x + width > bitmap->width)
      width = bitmap->width - src_x;

   /* clip to dest */
   if (dest_y + height <= 0)
   {
      return;
   }
   else if (dest_y < 0)
   {
      height += dest_y;
      src_y -= dest_y;
      dest_y = 0;
   }

   if (dest_y >= primary_buffer->height)
      return;
   if (dest_y + height > primary_buffer->height)
      height = primary_buffer->height - dest_y;

   if (dest_x + width <= 0)
   {
      return;
   }
   else if (dest_x < 0)
   {
      width += dest_x;
      src_x -= dest_x;
      dest_x = 0;
   }

   if (dest_x >= primary_buffer->width)
      return;
   if (dest_x + width > primary_buffer->width)
      width = primary_buffer->width - dest_x;

   src_ptr = bitmap->line[src_y] + src_x;
   dest_ptr = primary_buffer->line[dest_y] + dest_x;

   /* Avoid doing unnecessary indexed lookups */
   bitmap_pitch = bitmap->pitch;
   primary_pitch = primary_buffer->pitch;

   /* do the copy */
   while (height--)
   {
      vid_memcpy(dest_ptr, src_ptr, width);
      src_ptr += bitmap_pitch;
      dest_ptr += primary_pitch;
   }
}
void vid_shutdown(void) {
  if (NULL != primary_buffer)
     bmp_destroy(&primary_buffer);
}
