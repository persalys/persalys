// TaylorExpansionsMomentsResultWindow.hxx

#ifndef OTGUI_TAYLOREXPANSIONSMOMENTSRESULTWINDOW_H
#define OTGUI_TAYLOREXPANSIONSMOMENTSRESULTWINDOW_H

#include "otgui/ResultWindow.hxx"
#include "otgui/CentralTendencyItem.hxx"
#include "otgui/TaylorExpansionsMomentsResult.hxx"

#include <QComboBox>
#include <QLabel>

namespace OTGUI {
class TaylorExpansionsMomentsResultWindow : public OTguiSubWindow //ResultWindow
{
  Q_OBJECT

public:
  TaylorExpansionsMomentsResultWindow(CentralTendencyItem * item);

protected:
  void buildInterface();

public slots:
  void updateLabelsText(int indexOutput);

private:
  TaylorExpansionsMomentsResult result_;
  QComboBox * outputsComboBox_;
  QLabel * meanFirstOrderLabel_;
  QLabel * meanSecondOrderLabel_;
  QLabel * stdLabel_;
  QLabel * varianceLabel_;
};
}
#endif