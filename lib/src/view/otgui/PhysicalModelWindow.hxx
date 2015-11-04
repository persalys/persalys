// PhysicalModelWindow.hxx

#ifndef PHYSICALMODELWINDOW_H
#define PHYSICALMODELWINDOW_H

#include "otgui/OTguiSubWindow.hxx"
#include "otgui/PhysicalModelItem.hxx"
#include "otgui/PlotWidget.hxx"

#include <QTableView>
#include <QLineEdit>
#include <QGroupBox>
#include <QPushButton>
#include <QHBoxLayout>
#include <QLabel>

namespace OTGUI {
  
class PhysicalModelWindow : public OTguiSubWindow
{
  Q_OBJECT

public :
  PhysicalModelWindow(PhysicalModelItem * item);

protected:
  void buildInterface();

public slots:
  void addInputLine();
  void addOutputLine();
  void removeInputLine();
  void removeOutputLine();
  void inputDataChanged();
  void updateInputData(const InputCollection & inputs);
  void outputDataChanged();
  void updateOutputData(const OutputCollection & outputs);
  void methodChanged(int);
  void selectImportFileDialogRequested();
  void loadXML();
  void updateParametersWidgets(const OT::NumericalPointWithDescription & parameters = OT::NumericalPointWithDescription(0));
  void updatePlot();
  void variableChanged(const QModelIndex & currentIndex);
  void updateDistribution();

private:
  PhysicalModelItem * item_;
  QTableView * inputTableView_;
  InputTableModel * inputTableModel_;
  QTableView * outputTableView_;
  OutputTableModel * outputTableModel_;
  QLineEdit * XMLfileEdit_;
  QPushButton * loadButton_;
  QGroupBox * loadXMLFileBox_;
  PlotWidget * pdfPlot_;
  QGroupBox * paramEditor_;
  QHBoxLayout * parameterLayout_;
  QLabel * parameterValuesLabel_[2];
  QLineEdit * parameterValuesEdit_[2];
};
}
#endif