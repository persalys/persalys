//                                               -*- C++ -*-
/**
 *  @brief QMdiSubWindow to define a physical model
 *
 *  Copyright 2015-2018 EDF-Phimeca
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
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include "otgui/PythonPhysicalModelWindow.hxx"

#include "otgui/CodeDelegate.hxx"
#include "otgui/PhysicalModelWindowWidget.hxx"
#include "otgui/CodeModel.hxx"

#include <QVBoxLayout>
#include <QHeaderView>
#include <QSplitter>

namespace OTGUI
{

PythonPhysicalModelWindow::PythonPhysicalModelWindow(PhysicalModelDefinitionItem * item, QWidget * parent)
  : SubWindow(item, parent)
{
//  setFocusPolicy(Qt::ClickFocus);

  QVBoxLayout * widgetLayout = new QVBoxLayout(this);
  QString docLink("user_manual/graphical_interface/physical_model/user_manual_physical_model.html#vectpythonmodel");
  if (item->getPhysicalModel().hasMesh())
    docLink = "user_manual/graphical_interface/field_model/user_manual_field_model.html#fieldpythonmodel";
  widgetLayout->addWidget(new TitleLabel(tr("Python model"), docLink));

  QSplitter * horizontalSplitter = new QSplitter(Qt::Horizontal);

  // left side: code editor
  QTableView * codeView = new QTableView;
  codeView->setEditTriggers(QTableView::AllEditTriggers);
  codeView->horizontalHeader()->setStretchLastSection(true);
  codeView->verticalHeader()->setStretchLastSection(true);
  codeView->horizontalHeader()->hide();
  codeView->verticalHeader()->hide();
  codeView->setItemDelegate(new CodeDelegate(codeView));

  CodeModel * codeModel = new CodeModel(item->getPhysicalModel(), codeView);
  codeView->setModel(codeModel);
  codeView->openPersistentEditor(codeModel->index(0, 0));
  connect(item, SIGNAL(codeChanged()), codeModel, SLOT(updateData()));

  horizontalSplitter->addWidget(codeView);

  // right side:
  // - tables
  PhysicalModelWindowWidget * tablesWidget = new PhysicalModelWindowWidget(item);
  connect(codeModel, SIGNAL(variablesChanged()), tablesWidget, SIGNAL(updateInputTableData()));
  connect(codeModel, SIGNAL(variablesChanged()), tablesWidget, SIGNAL(updateOutputTableData()));
  connect(codeModel, SIGNAL(variablesChanged()), tablesWidget, SIGNAL(resetMessageLabel()));

  horizontalSplitter->addWidget(tablesWidget);

  ////////////////
  widgetLayout->addWidget(horizontalSplitter, 1);
}
}
