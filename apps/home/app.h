#ifndef HOME_APP_H
#define HOME_APP_H

#include <escher.h>
#include "controller.h"

namespace Home {

class App : public ::App {
public:
  class Descriptor : public ::App::Descriptor {
  public:
    const char * name() override;
    const char * upperName() override;
  };
  class Snapshot : public ::App::Snapshot, public SelectableTableViewDataSource {
  public:
    App * unpack(Container * container) override;
    Descriptor * descriptor() override;
  };
  static App * app() {
    return static_cast<App *>(Container::activeApp());
  }
  Snapshot * snapshot() const {
    return static_cast<Snapshot *>(::App::snapshot());
  }
private:
  App(Snapshot * snapshot);
  Controller m_controller;
};

}

#endif
