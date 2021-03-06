#include "app.h"
#include "../apps_container.h"
#include "sequence_icon.h"

using namespace Poincare;

namespace Sequence {

I18n::Message App::Descriptor::name() {
  return I18n::Message::SequenceApp;
}

I18n::Message App::Descriptor::upperName() {
  return I18n::Message::SequenceAppCapital;
}

const Image * App::Descriptor::icon() {
  return ImageStore::SequenceIcon;
}

App::Snapshot::Snapshot() :
  Shared::FunctionApp::Snapshot::Snapshot(),
  m_sequenceStore(),
  m_graphRange(&m_cursor)
{
}

App * App::Snapshot::unpack(Container * container) {
  return new App(container, this);
}

void App::Snapshot::reset() {
  FunctionApp::Snapshot::reset();
  m_graphRange.setDefault();
  m_sequenceStore.removeAll();
}

App::Descriptor * App::Snapshot::descriptor() {
  static Descriptor descriptor;
  return &descriptor;
}

SequenceStore * App::Snapshot::sequenceStore() {
  return &m_sequenceStore;
}

CurveViewRange * App::Snapshot::graphRange() {
  return &m_graphRange;
}

void App::Snapshot::tidy() {
  m_sequenceStore.tidy();
  m_graphRange.setDelegate(nullptr);
}

App::App(Container * container, Snapshot * snapshot) :
  FunctionApp(container, snapshot, &m_inputViewController),
  m_nContext(((AppsContainer *)container)->globalContext()),
  m_listController(&m_listFooter, snapshot->sequenceStore(), &m_listHeader, &m_listFooter),
  m_listFooter(&m_listHeader, &m_listController, &m_listController, ButtonRowController::Position::Bottom, ButtonRowController::Style::EmbossedGrey),
  m_listHeader(nullptr, &m_listFooter, &m_listController),
  m_listStackViewController(&m_tabViewController, &m_listHeader),
  m_graphController(&m_graphAlternateEmptyViewController, snapshot->sequenceStore(), snapshot->graphRange(), snapshot->cursor(), snapshot->modelVersion(), snapshot->rangeVersion(), snapshot->angleUnitVersion(), &m_graphHeader),
  m_graphAlternateEmptyViewController(&m_graphHeader, &m_graphController, &m_graphController),
  m_graphHeader(&m_graphStackViewController, &m_graphAlternateEmptyViewController, &m_graphController),
  m_graphStackViewController(&m_tabViewController, &m_graphHeader),
  m_valuesController(&m_valuesAlternateEmptyViewController, snapshot->sequenceStore(), snapshot->interval(), &m_valuesHeader),
  m_valuesAlternateEmptyViewController(&m_valuesHeader, &m_valuesController, &m_valuesController),
  m_valuesHeader(nullptr, &m_valuesAlternateEmptyViewController, &m_valuesController),
  m_valuesStackViewController(&m_tabViewController, &m_valuesHeader),
  m_tabViewController(&m_inputViewController, snapshot, &m_listStackViewController, &m_graphStackViewController, &m_valuesStackViewController),
  m_inputViewController(&m_modalViewController, &m_tabViewController, &m_listController)
{
}

InputViewController * App::inputViewController() {
  return &m_inputViewController;
}

Context * App::localContext() {
  if (m_tabViewController.activeTab() == 0) {
    return &m_nContext;
  }
  return TextFieldDelegateApp::localContext();
}

const char * App::XNT() {
  return "n";
}

}
