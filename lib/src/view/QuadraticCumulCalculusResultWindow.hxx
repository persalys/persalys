// QuadraticCumulCalculusResultWindow.hxx

#ifndef QUADRATICCUMULCALCULUSRESULTWINDOW_H
#define QUADRATICCUMULCALCULUSRESULTWINDOW_H

#include "ResultWindow.hxx"
#include "DistributionAnalysisItem.hxx"
#include "QuadraticCumulResult.hxx"

#include <QComboBox>
#include <QLabel>

namespace OTGUI {
class QuadraticCumulCalculusResultWindow : public OTguiSubWindow //ResultWindow
{
  Q_OBJECT

public:
  QuadraticCumulCalculusResultWindow(DistributionAnalysisItem * item);

protected:
  void buildInterface();

public slots:
  void updateLabelsText(int indexOutput);

private:
  QuadraticCumulResult result_;
  QComboBox * outputsComboBox_;
  QLabel * meanFirstOrderLabel_;
  QLabel * meanSecondOrderLabel_;
  QLabel * stdLabel_;
  QLabel * varianceLabel_;
};
}
#endif