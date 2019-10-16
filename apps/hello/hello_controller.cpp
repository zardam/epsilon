#include "hello_controller.h"

namespace Hello {

HelloController::HelloController() :
  ViewController(nullptr),
  m_helloView()
{
}

View * HelloController::view() {
  return &m_helloView;
}

}
