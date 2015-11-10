// QuadraticCumulResultWindow.hxx

#ifndef QUADRATICCUMULRESULTWINDOW_H
#define QUADRATICCUMULRESULTWINDOW_H

#include "otgui/ResultWindow.hxx"
#include "otgui/CentralTendencyItem.hxx"
#include "otgui/QuadraticCumulResult.hxx"

#include <QComboBox>
#include <QLabel>

namespace OTGUI {
class QuadraticCumulResultWindow : public OTguiSubWindow //ResultWindow
{
  Q_OBJECT

public:
  QuadraticCumulResultWindow(CentralTendencyItem * item);

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