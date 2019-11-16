extern "C" {
#include "modkandinsky.h"
#include <py/objtuple.h>
#include <py/runtime.h>
}
#include <kandinsky.h>
#include "port.h"

static KDColor ColorForTuple(mp_obj_t tuple) {
  if (MP_OBJ_IS_SMALL_INT(tuple)) // BP change: accept int for color
    return MP_OBJ_SMALL_INT_VALUE(tuple);

    size_t len;
    mp_obj_t * elem;

    mp_obj_get_array(tuple, &len, &elem);
    if (len != 3) {
      mp_raise_TypeError("color needs 3 components");
    }

    return KDColor::RGB888(
      mp_obj_get_int(elem[0]),
      mp_obj_get_int(elem[1]),
      mp_obj_get_int(elem[2])
    );
}

static mp_obj_t TupleForRGB(uint8_t r, uint8_t g, uint8_t b) {
  mp_obj_tuple_t * t = static_cast<mp_obj_tuple_t *>(MP_OBJ_TO_PTR(mp_obj_new_tuple(3, NULL)));
  t->items[0] = MP_OBJ_NEW_SMALL_INT(r);
  t->items[1] = MP_OBJ_NEW_SMALL_INT(g);
  t->items[2] = MP_OBJ_NEW_SMALL_INT(b);
  return MP_OBJ_FROM_PTR(t);
}

/* KDIonContext::sharedContext needs to be set to the wanted Rect before
 * calling kandinsky_get_pixel, kandinsky_set_pixel and kandinsky_draw_string.
 * We do this here with displaySandbox(), which pushes the SandboxController on
 * the stackViewController and forces the window to redraw itself.
 * KDIonContext::sharedContext is set to the frame of the last object drawn. */

mp_obj_t modkandinsky_color(mp_obj_t red, mp_obj_t green, mp_obj_t blue) {
  return TupleForRGB(
    mp_obj_get_int(red),
    mp_obj_get_int(green),
    mp_obj_get_int(blue)
  );
}

/* Calling ExecutionEnvironment::displaySandbox() hides the console and switches
 * to another mode. So it's a good idea to retrieve and handle input parameters
 * before calling displaySandbox, otherwise error messages (such as TypeError)
 * won't be visible until the user comes back to the console screen. */

mp_obj_t modkandinsky_get_pixel(mp_obj_t x, mp_obj_t y) {
  KDPoint point(mp_obj_get_int(x), mp_obj_get_int(y));
  KDColor c = KDIonContext::sharedContext()->getPixel(point);
  return TupleForRGB(c.red(), c.green(), c.blue());
}

mp_obj_t modkandinsky_set_pixel(mp_obj_t x, mp_obj_t y, mp_obj_t color) {
  KDPoint point(mp_obj_get_int(x), mp_obj_get_int(y));
  KDColor kdColor = ColorForTuple(color);
  MicroPython::ExecutionEnvironment::currentExecutionEnvironment()->displaySandbox();
  KDIonContext::sharedContext()->setPixel(point, kdColor);
  return mp_const_none;
}

mp_obj_t modkandinsky_draw_string(size_t n_args, const mp_obj_t * args) {
  const char * text = mp_obj_str_get_str(args[0]);
  KDPoint point(mp_obj_get_int(args[1]), mp_obj_get_int(args[2]));
  KDColor textColor = (n_args >= 4) ? ColorForTuple(args[3]) : KDColorBlack;
  KDColor backgroundColor = (n_args >= 5) ? ColorForTuple(args[4]) : KDColorWhite;
  MicroPython::ExecutionEnvironment::currentExecutionEnvironment()->displaySandbox();
  KDIonContext::sharedContext()->drawString(text, point, KDFont::LargeFont, textColor, backgroundColor);
  return mp_const_none;
}

mp_obj_t modkandinsky_fill_rect(size_t n_args, const mp_obj_t * args) {
  KDRect rect(
    mp_obj_get_int(args[0]),
    mp_obj_get_int(args[1]),
    mp_obj_get_int(args[2]),
    mp_obj_get_int(args[3])
  );
  KDColor color = ColorForTuple(args[4]);
  MicroPython::ExecutionEnvironment::currentExecutionEnvironment()->displaySandbox();
  KDIonContext::sharedContext()->fillRect(rect, color);
  return mp_const_none;
}

/* C-- SDK (c) B. Parisse, 2019 */
mp_obj_t modkandinsky_Pause(mp_obj_t x) {
  double d=mp_obj_get_float(x);
  numworks_wait_1ms(d*1000);
  return mp_const_none;
}

#define LCD_WIDTH_PX 320
#define LCD_HEIGHT_PX 222

void numworks_set_pixel(int x, int y, int color) {
  if (x<0 || x>=LCD_WIDTH_PX || y<0 || y>=LCD_HEIGHT_PX)
    return;
  auto ctx=KDIonContext::sharedContext();
  KDRect save=ctx->m_clippingRect;
  KDPoint o=ctx->m_origin;
  ctx->setClippingRect(KDRect(0,0,320,240));
  ctx->setOrigin(KDPoint(0,0));
  KDColor c(color);
  KDPoint point(x,y+18);
  KDIonContext::sharedContext()->pushRect(KDRect(point, 1, 1), &c);
  ctx->setClippingRect(save);
  ctx->setOrigin(o);  
}

void numworks_fill_rect(int x,int y,int w,int h,int c){
  KDColor color = c;
  auto ctx=KDIonContext::sharedContext();
  KDRect save=ctx->m_clippingRect;
  KDPoint o=ctx->m_origin;
  ctx->setClippingRect(KDRect(0,0,320,240));
  ctx->setOrigin(KDPoint(0,0));
#if 1
  if (x<0){
    w += x;
    x=0;
  }
  if (y<0){
    h += y;
    y=0;
  }
  if (h+y>=LCD_HEIGHT_PX)
    h=LCD_HEIGHT_PX-y;
  if (x+w>=LCD_WIDTH_PX)
    w=LCD_WIDTH_PX-x;
  if (h<=0 || w<=0)
    return;
  KDRect rect(x,y+18,w,h);
  KDIonContext::sharedContext()->pushRectUniform(rect,color);
#else
  KDRect rect(x,y,w,h); 
  KDIonContext::sharedContext()->fillRect(rect, color);
#endif
  ctx->setClippingRect(save);
  ctx->setOrigin(o);  
}

int numworks_get_pixel(int x, int y) {
  KDPoint point(x,y);
  KDColor c = KDIonContext::sharedContext()->getPixel(point);
  return c;
}

int numworks_draw_string(int x,int y,int c,int bg,const char * text,bool fake){
  auto ptr=MicroPython::ExecutionEnvironment::currentExecutionEnvironment();
  KDPoint point(x,y);
  if (ptr)
    ptr->displaySandbox();
  auto ctx=KDIonContext::sharedContext();
  KDRect save=ctx->m_clippingRect;
  KDPoint o=ctx->m_origin;
  ctx->setClippingRect(KDRect(0,18,320,fake?0:222));
  ctx->setOrigin(KDPoint(0,18));
  point=KDIonContext::sharedContext()->drawString(text, point, KDFont::LargeFont, c, bg);
  ctx->setClippingRect(save);
  ctx->setOrigin(o);
  return point.x();
}

int numworks_draw_string_small(int x,int y,int c,int bg,const char * text,bool fake){
  auto ptr=MicroPython::ExecutionEnvironment::currentExecutionEnvironment();
  KDPoint point(x,y);
  if (ptr)
    ptr->displaySandbox();
  auto ctx=KDIonContext::sharedContext();
  KDRect save=ctx->m_clippingRect;
  KDPoint o=ctx->m_origin;
  ctx->setClippingRect(KDRect(0,18,320,fake?0:222));
  ctx->setOrigin(KDPoint(0,18));
  point=ctx->drawString(text, point, KDFont::SmallFont, c, bg);
  ctx->setClippingRect(save);
  ctx->setOrigin(o);
  return point.x();
}

void numworks_hide_graph(){
  auto ptr=MicroPython::ExecutionEnvironment::currentExecutionEnvironment();
  if (ptr)
    ptr->hideSandbox();
}

void numworks_show_graph(){
  auto ptr=MicroPython::ExecutionEnvironment::currentExecutionEnvironment();
  if (ptr)
    ptr->displaySandbox();
}

// Python module get_key() addition
mp_obj_t modkandinsky_get_key() {
  micropython_port_interrupt_if_needed();
  int key=getkey(false); // no suspend
  return mp_obj_new_int(key);
}
