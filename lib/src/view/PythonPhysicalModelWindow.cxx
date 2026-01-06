//                                               -*- C++ -*-
/**
 *  @brief QMdiSubWindow to define a physical model
 *
 *  Copyright 2015-2025 EDF-Phimeca
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
#include "persalys/PythonPhysicalModelWindow.hxx"

#include "persalys/CodeDelegate.hxx"
#include "persalys/PhysicalModelWindowWidget.hxx"
#include "persalys/CodeModel.hxx"
#include "persalys/CheckModelButtonGroup.hxx"

#include <QVBoxLayout>
#include <QHeaderView>
#include <QSplitter>

namespace PERSALYS
{

PythonPhysicalModelWindow::PythonPhysicalModelWindow(PhysicalModelItem * item, QWidget * parent)
  : SubWindow(item, parent)
{
  auto * widgetLayout = new QVBoxLayout(this);
  QString docLink("user_manual/graphical_interface/physical_model/user_manual_physical_model.html#vectpythonmodel");
  if (item->getPhysicalModel().hasMesh())
    docLink = "user_manual/graphical_interface/field_model/user_manual_field_model.html#fieldpythonmodel";
  widgetLayout->addWidget(new TitleLabel(tr("Python model"), docLink));

  auto * horizontalSplitter = new QSplitter(Qt::Horizontal);

  // left side: code editor
  auto * codeView = new QTableView;
  codeView->setEditTriggers(QTableView::AllEditTriggers);
  codeView->horizontalHeader()->setStretchLastSection(true);
  codeView->verticalHeader()->setStretchLastSection(true);
  codeView->horizontalHeader()->hide();
  codeView->verticalHeader()->hide();
  codeView->setItemDelegate(new CodeDelegate(codeView));

  auto * codeModel = new CodeModel(item->getPhysicalModel(), codeView);
  codeView->setModel(codeModel);
  codeView->openPersistentEditor(codeModel->index(0, 0));
  connect(item, &PhysicalModelItem::codeChanged, codeModel, &CodeModel::updateData);

  horizontalSplitter->addWidget(codeView);

  // right side:
  // - tables
  auto * tablesWidget = new PhysicalModelWindowWidget(item);

  connect(codeModel, &CodeModel::variablesChanged, tablesWidget, &PhysicalModelWindowWidget::updateInputTableData);
  connect(codeModel, &CodeModel::variablesChanged, tablesWidget, &PhysicalModelWindowWidget::updateOutputTableData);

  horizontalSplitter->addWidget(tablesWidget);

  ////////////////
  widgetLayout->addWidget(horizontalSplitter, 1);

  // buttons
  auto * buttons = new CheckModelButtonGroup(this, !item->getPhysicalModel().hasMesh());
  connect(buttons, &CheckModelButtonGroup::evaluateOutputsRequested, tablesWidget, &PhysicalModelWindowWidget::evaluateOutputsRequested);
  connect(buttons, &CheckModelButtonGroup::evaluateGradientRequested, tablesWidget, &PhysicalModelWindowWidget::evaluateGradientRequested);

  connect(codeModel, &CodeModel::variablesChanged, buttons->getErrorMessageWidget(), &ErrorWidget::reset);
  connect(codeModel, &CodeModel::errorMessageChanged, buttons->getErrorMessageWidget(), &ErrorWidget::setFramelessErrorMessage);
  connect(tablesWidget, &PhysicalModelWindowWidget::errorMessageChanged, buttons->getErrorMessageWidget(), &ErrorWidget::setFramelessErrorMessage);
  connect(tablesWidget, &PhysicalModelWindowWidget::resetMessageLabel, buttons->getErrorMessageWidget(), &ErrorWidget::reset);
  widgetLayout->addWidget(buttons);

}
}
