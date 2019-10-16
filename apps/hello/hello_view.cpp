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
  ctx->drawString("KhiCAS code de sortie", KDPointZero);
  ctx->drawString(str, KDPoint(18,18));
  ctx->drawString("Menu principal HOME", KDPoint(0,36));
  ctx->drawString("De la, HOME relance Xcas", KDPoint(0,54));

}

}
