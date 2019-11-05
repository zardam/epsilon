#ifndef APPS_HELLO_VIEW_H
#define APPS_HELLO_VIEW_H

#include <escher.h>

namespace Hello {

class HelloView : public View {
public:
  HelloView();
  void drawRect(KDContext * ctx, KDRect rect) const override;
};

}

#endif
