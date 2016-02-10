//                                               -*- C++ -*-
/**
 *  @brief QMdiSubWindow to define a physical model
 *
 *  Copyright 2015-2016 EDF
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#ifndef OTGUI_PHYSICALMODELWINDOW_HXX
#define OTGUI_PHYSICALMODELWINDOW_HXX

#include "otgui/OTguiSubWindow.hxx"
#include "otgui/PhysicalModelItem.hxx"
#include "otgui/PlotWidget.hxx"
#include "otgui/CodeModel.hxx"

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
  void updateCodeModel();
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
  QLabel * XMLErrorMessage_;
  QGroupBox * loadXMLFileBox_;
  CodeModel * codeModel_;
  QTableView * codeView_;
  QGroupBox * pythonDefinitionBox_;
};
}
#endif