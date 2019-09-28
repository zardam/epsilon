#ifndef APPS_EXTERNAL_APP_H
#define APPS_EXTERNAL_APP_H

#include <escher.h>

class ExternalApp : public App {
public:
  class Descriptor : public App::Descriptor {
  public:
    const char * name() override;
    const char * upperName() override;
    const Image * icon() override;

    char filename[Ion::Archive::MaxNameLength];
  };
  class Snapshot : public App::Snapshot {
  public:
    App * unpack(Container * container) override;
    Descriptor * descriptor() override;
  private:
    Descriptor m_descriptor;
  };
private:
  ExternalApp(ExternalApp::Snapshot * snapshot);
};

#endif