#ifndef APPS_HELLO_CONTROLLER_H
#define APPS_HELLO_CONTROLLER_H

#include <escher.h>
#include "hello_view.h"

namespace Hello {

class HelloController : public ViewController {
public:
  HelloController();
  View * view() override;
private:
  HelloView m_helloView;
};

}

#endif
