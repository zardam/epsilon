#include "app.h"
#include "../apps_container.h"
#include "hello_icon.h"
#include "../i18n.h"

namespace Hello {

I18n::Message App::Descriptor::name() {
  return I18n::Message::HelloApp;
}

I18n::Message App::Descriptor::upperName() {
  return I18n::Message::HelloAppCapital;
}

const Image * App::Descriptor::icon() {
  return ImageStore::HelloIcon;
}

App * App::Snapshot::unpack(Container * container) {
  return new (container->currentAppBuffer()) App(container, this);
}

void App::Snapshot::reset() {
}

App::Descriptor * App::Snapshot::descriptor() {
  static Descriptor descriptor;
  return &descriptor;
}

void App::didBecomeActive(Window * window) {
  ::App::didBecomeActive(window);
}

void App::willBecomeInactive() {
  ::App::willBecomeInactive();
}


bool App::processEvent(Ion::Events::Event e) {
  return ::App::processEvent(e);
}

App::App(Container * container, Snapshot * snapshot) :
  ::App(snapshot, &m_helloController),
  m_appsContainer((AppsContainer*) container)
{
}

}
