#ifndef HELLO_APP_H
#define HELLO_APP_H

#include <escher.h>
#include "hello_controller.h"
#include "../apps_container.h"

namespace Hello {

class App : public ::App  {
public:
  class Descriptor : public ::App::Descriptor {
  public:
    I18n::Message name() override;
    I18n::Message upperName() override;
    const Image * icon() override;
  };
  class Snapshot : public ::App::Snapshot {
  public:
    App * unpack(Container * container) override;
    void reset() override;
    Descriptor * descriptor() override;
  };
  bool processEvent(Ion::Events::Event) override;
  virtual void didBecomeActive(Window * window);
  virtual void willBecomeInactive();
private:
  App(Container * container, Snapshot * snapshot);
  HelloController m_helloController;
  AppsContainer * m_appsContainer;
};

}

#endif
