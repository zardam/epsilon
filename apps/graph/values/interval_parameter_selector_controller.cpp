#include "interval_parameter_selector_controller.h"
#include "../../shared/interval_parameter_controller.h"
#include "../app.h"
#include <apps/i18n.h>
#include <assert.h>

namespace Graph {

IntervalParameterSelectorController::IntervalParameterSelectorController() :
  ViewController(nullptr),
  m_selectableTableView(this, this, this)
{
}

const char * IntervalParameterSelectorController::title() {
  return I18n::translate(I18n::Message::IntervalSet);
}

void IntervalParameterSelectorController::viewDidDisappear() {
  /* Deselect the table properly because it needs to be relayouted the next time
   * it appears: the number of rows might change according to the plot type. */
  m_selectableTableView.deselectTable(false);
  m_selectableTableView.setFrame(KDRectZero);
}

void IntervalParameterSelectorController::didBecomeFirstResponder() {
  if (selectedRow() < 0) {
    selectCellAtLocation(0, 0);
  }
  Container::activeApp()->setFirstResponder(&m_selectableTableView);
}

bool IntervalParameterSelectorController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK || event == Ion::Events::EXE || event == Ion::Events::Right) {
    StackViewController * stack = (StackViewController *)parentResponder();
    Shared::IntervalParameterController * controller = App::app()->valuesController()->intervalParameterController();
    Shared::CartesianFunction::PlotType plotType = plotTypeAtRow(selectedRow());
    controller->setTitle(messageForType(plotType));
    controller->setInterval(App::app()->intervalForType(plotType));
    stack->push(controller);
    return true;
  }
  return false;
}

int IntervalParameterSelectorController::numberOfRows() {
  int rowCount = 0;
  int plotTypeIndex = 0;
  Shared::CartesianFunction::PlotType plotType;
  while (plotTypeIndex < Shared::CartesianFunction::k_numberOfPlotTypes) {
    plotType = static_cast<Shared::CartesianFunction::PlotType>(plotTypeIndex);
    bool plotTypeIsShown = App::app()->functionStore()->numberOfActiveFunctionsOfType(plotType) > 0;
    rowCount += plotTypeIsShown;
    plotTypeIndex++;
  }
  return rowCount;
}

HighlightCell * IntervalParameterSelectorController::reusableCell(int index) {
  assert(0 <= index && index < reusableCellCount());
  return m_intervalParameterCell + index;
}

int IntervalParameterSelectorController::reusableCellCount() {
  return Shared::CartesianFunction::k_numberOfPlotTypes;
}

void IntervalParameterSelectorController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  assert(0 <= index && index < numberOfRows());
  Shared::CartesianFunction::PlotType plotType = plotTypeAtRow(index);
  static_cast<MessageTableCellWithChevron *>(cell)->setMessage(messageForType(plotType));
}

Shared::CartesianFunction::PlotType IntervalParameterSelectorController::plotTypeAtRow(int j) const {
  int rowCount = 0;
  int plotTypeIndex = 0;
  Shared::CartesianFunction::PlotType plotType;
  while (plotTypeIndex < Shared::CartesianFunction::k_numberOfPlotTypes) {
    plotType = static_cast<Shared::CartesianFunction::PlotType>(plotTypeIndex);
    bool plotTypeIsShown = App::app()->functionStore()->numberOfActiveFunctionsOfType(plotType) > 0;
    if (plotTypeIsShown && rowCount == j) {
      break;
    }
    rowCount += plotTypeIsShown;
    plotTypeIndex++;
  }
  assert(rowCount == j);
  return plotType;
}

I18n::Message IntervalParameterSelectorController::messageForType(Shared::CartesianFunction::PlotType plotType) {
  constexpr I18n::Message message[Shared::CartesianFunction::k_numberOfPlotTypes] = {
    I18n::Message::IntervalX,
    I18n::Message::IntervalTheta,
    I18n::Message::IntervalT
  };
  return message[static_cast<size_t>(plotType)];
}

}