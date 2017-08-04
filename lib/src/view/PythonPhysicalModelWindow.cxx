//                                               -*- C++ -*-
/**
 *  @brief QMdiSubWindow to define a physical model
 *
 *  Copyright 2015-2017 EDF-Phimeca
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
#include "otgui/PythonPhysicalModelWindow.hxx"

#include "otgui/CodeDelegate.hxx"

#include <QVBoxLayout>
#include <QHeaderView>
#include <QSplitter>

using namespace OT;

namespace OTGUI {

PythonPhysicalModelWindow::PythonPhysicalModelWindow(PhysicalModelDefinitionItem * item, QWidget * parent)
  : OTguiSubWindow(item, parent)
  , physicalModel_(item->getPhysicalModel())
  , codeModel_(0)
  , codeView_(0)
  , tablesWidget_(0)
{
  connect(item, SIGNAL(codeChanged()), this, SLOT(updateCodeModel()));

  setWindowTitle(tr("Python physical model"));

  setFocusPolicy(Qt::ClickFocus);

  QSplitter * horizontalSplitter = new QSplitter(Qt::Horizontal);

  codeView_ = new QTableView;
  codeView_->setEditTriggers(QTableView::AllEditTriggers);
  codeView_->horizontalHeader()->setStretchLastSection(true);
  codeView_->verticalHeader()->setStretchLastSection(true);
  codeView_->horizontalHeader()->hide();
  codeView_->verticalHeader()->hide();
  codeView_->setItemDelegate(new CodeDelegate);
  horizontalSplitter->addWidget(codeView_);

  QWidget * rightSideWidget = new QWidget;
  QVBoxLayout * vBoxLayout = new QVBoxLayout(rightSideWidget);

  tablesWidget_ = new PhysicalModelWindowWidget(item);
  connect(tablesWidget_, SIGNAL(errorMessageChanged(QString)), this, SLOT(setErrorMessage(QString)));
  vBoxLayout->addWidget(tablesWidget_);

  errorMessageLabel_ = new QLabel;
  errorMessageLabel_->setWordWrap(true);
  vBoxLayout->addWidget(errorMessageLabel_);

  horizontalSplitter->addWidget(rightSideWidget);

  ////////////////
  setWidget(horizontalSplitter);
  updateCodeModel();
}


void PythonPhysicalModelWindow::updateCodeModel()
{
  if (codeModel_)
    delete codeModel_;
  codeModel_ = new CodeModel(physicalModel_, codeView_);
  codeView_->setModel(codeModel_);
  codeView_->openPersistentEditor(codeModel_->index(0, 0));
  connect(codeModel_, SIGNAL(variablesChanged()), tablesWidget_, SLOT(updateInputTableModel()));
  connect(codeModel_, SIGNAL(variablesChanged()), tablesWidget_, SLOT(updateOutputTableModel()));
}
}
