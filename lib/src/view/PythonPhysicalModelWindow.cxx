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
#include "otgui/PythonPhysicalModelWindow.hxx"

#include "otgui/CodeDelegate.hxx"
#include "otgui/PhysicalModelWindowWidget.hxx"

#include <QHBoxLayout>
#include <QHeaderView>

using namespace OT;

namespace OTGUI {

PythonPhysicalModelWindow::PythonPhysicalModelWindow(PhysicalModelItem * item)
  : OTguiSubWindow(item)
  , physicalModel_(item->getPhysicalModel())
  , codeModel_(0)
{
  connect(item, SIGNAL(codeChanged()), this, SLOT(updateCodeModel()));

  QWidget * mainWidget = new QWidget;
  QHBoxLayout * mainLayout = new QHBoxLayout(mainWidget);

  codeView_ = new QTableView;
  codeView_->setEditTriggers(QTableView::AllEditTriggers);
  codeView_->horizontalHeader()->setStretchLastSection(true);
  codeView_->verticalHeader()->setStretchLastSection(true);
  codeView_->horizontalHeader()->hide();
  codeView_->verticalHeader()->hide();
  codeView_->setItemDelegate(new CodeDelegate);
  mainLayout->addWidget(codeView_);
  updateCodeModel();

  QVBoxLayout * vBoxLayout = new QVBoxLayout;
  PhysicalModelWindowWidget * widget = new PhysicalModelWindowWidget(item);
  connect(widget, SIGNAL(errorMessageChanged(QString)), this, SLOT(setErrorMessage(QString)));
  vBoxLayout->addWidget(widget);

  errorMessageLabel_ = new QLabel;
  errorMessageLabel_->setWordWrap(true);
  vBoxLayout->addWidget(errorMessageLabel_);

  mainLayout->addLayout(vBoxLayout);

  ////////////////
  setWidget(mainWidget);
}


void PythonPhysicalModelWindow::updateCodeModel()
{
  if (codeModel_)
    delete codeModel_;
  codeModel_ = new CodeModel(physicalModel_);
  codeView_->setModel(codeModel_);
}
}