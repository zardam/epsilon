#ifndef SETTINGS_APP_H
#define SETTINGS_APP_H

#include "main_controller.h"
#include "../shared/text_field_delegate_app.h"

namespace Settings {

class App : public Shared::TextFieldDelegateApp {
public:
  class Descriptor : public ::App::Descriptor {
  public:
    const char * name() override;
    const char * upperName() override;
    const Image * icon() override;
  };
  class Snapshot : public ::App::Snapshot {
  public:
    App * unpack(Container * container) override;
    Descriptor * descriptor() override;
  };
private:
  App(Snapshot * snapshot);
  MainController m_mainController;
  StackViewController m_stackViewController;
};

}

#endif
