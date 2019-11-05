#include "hello_view.h"
#include <assert.h>
  extern "C" const char * caseval(const char *);

namespace Hello {

HelloView::HelloView() :
  View()
{
}

void HelloView::drawRect(KDContext * ctx, KDRect rect) const {
  const char * str=caseval("*");
  ctx->fillRect(bounds(), KDColorWhite);
  ctx->drawString("   HOME: Menu principal", KDPoint(0,0));
  ctx->drawString("Depuis HOME, HOME lance Khicas", KDPoint(0,18));
  int y=54;
  ctx->drawString("   KhiCAS 1.5.0, (c) 2019 par", KDPoint(0,y)); y+=18;
  ctx->drawString("B. Parisse et R. De Graeve", KDPoint(0,y)); y+=18;
  ctx->drawString("Licence GNU GPL version 2", KDPoint(0,y)); y+=18;
  ctx->drawString("Port Numworks Damien Nicolet", KDPoint(0,y)); y+=18;
  ctx->drawString("Merci a Jean-Baptiste Boric,", KDPoint(0,y)); y+=18;
  ctx->drawString("M. Friess et a l'equipe tiplanet", KDPoint(0,y)); y+=18;
  ctx->drawString("pour l'aide apportee au portage", KDPoint(0,y)); y+=18;
  ctx->drawString("de Khicas sur Numworks.", KDPoint(0,y)); y+=18;
  ctx->drawString("Table periodique par M. Friess", KDPoint(0,y)); y+=18;

}

}
