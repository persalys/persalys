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
#include <QTextEdit>

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
  void evaluateOutputs();
  void updateInputTableModel();
  void updateOutputTableModel();
  void updateMethodWidgets(int);
  void methodChanged(int);
  void selectImportFileDialogRequested();
  void loadXML();

signals:
  void physicalModelChanged(PhysicalModel);

private:
  PhysicalModel physicalModel_;
  QTableView * inputTableView_;
  InputTableModel * inputTableModel_;
  QPushButton * addInputLineButton_;
  QPushButton * removeInputLineButton_;
  QTableView * outputTableView_;
  OutputTableModel * outputTableModel_;
  QPushButton * addOutputLineButton_;
  QPushButton * removeOutputLineButton_;
  QPushButton * evaluateOutputsButton_;
  QLineEdit * XMLfileNameEdit_;
  QPushButton * loadButton_;
  QGroupBox * loadXMLFileBox_;
  QGroupBox * pythonDefinitionBox_;
  QTextEdit * pythonCodeEdit_;
};
}
#endif