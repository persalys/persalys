// MonteCarloReliabilityResultWindow.hxx

#ifndef MONTECARLORELIABILITYRESULTWINDOW_H
#define MONTECARLORELIABILITYRESULTWINDOW_H

#include "otgui/ResultWindow.hxx"
#include "otgui/ReliabilityAnalysisItem.hxx"
#include "SimulationResult.hxx"

namespace OTGUI {
class MonteCarloReliabilityResultWindow : public OTguiSubWindow //ResultWindow
{
  Q_OBJECT

public:
  MonteCarloReliabilityResultWindow(ReliabilityAnalysisItem * item);

protected:
  void buildInterface();

private:
  OT::SimulationResult result_;
};
}
#endif