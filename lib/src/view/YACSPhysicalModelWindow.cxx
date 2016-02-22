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
#include "otgui/YACSPhysicalModelWindow.hxx"

#include "otgui/YACSPhysicalModel.hxx"
#include "otgui/PhysicalModelWindowWidget.hxx"

#include <QFileDialog>
#include <QHeaderView>
#include <QVBoxLayout>
#include <QPushButton>

using namespace OT;

namespace OTGUI {

YACSPhysicalModelWindow::YACSPhysicalModelWindow(PhysicalModelItem * item)
  : OTguiSubWindow(item)
  , physicalModel_(item->getPhysicalModel())
{
  QWidget * mainWidget = new QWidget;
  QVBoxLayout * mainLayout = new QVBoxLayout(mainWidget);

  // Widgets to load XML file
  QHBoxLayout * fieldsLayout = new QHBoxLayout;

  fieldsLayout->addWidget(new QLabel("Data file"));

  XMLfileNameEdit_ = new QLineEdit;
  XMLfileNameEdit_->setText(dynamic_cast<YACSPhysicalModel*>(&*physicalModel_.getImplementation())->getXMLFileName().c_str());
  fieldsLayout->addWidget(XMLfileNameEdit_);

  QPushButton * selectFileButton = new QPushButton(tr("Search file"));
  connect(selectFileButton, SIGNAL(clicked()), this, SLOT(selectImportFileDialogRequested()));
  fieldsLayout->addWidget(selectFileButton);

  mainLayout->addLayout(fieldsLayout);

  PhysicalModelWindowWidget * widget = new PhysicalModelWindowWidget(item);
  connect(widget, SIGNAL(errorMessageChanged(QString)), this, SLOT(setErrorMessage(QString)));
  mainLayout->addWidget(widget);

  errorMessageLabel_ = new QLabel;
  errorMessageLabel_->setWordWrap(true);
  mainLayout->addWidget(errorMessageLabel_);

  ////////////////
  setWidget(mainWidget);
}


void YACSPhysicalModelWindow::selectImportFileDialogRequested()
{
  QString xmlPath;
  QString fileName = QFileDialog::getOpenFileName(this, tr("Data to import..."),
                     xmlPath,
                     tr("Data files (*.xml);;"));

  if (!fileName.isEmpty())
  {
    XMLfileNameEdit_->setText(fileName);
    try
    {
      dynamic_cast<YACSPhysicalModel*>(&*physicalModel_.getImplementation())->setXMLFileName(fileName.toStdString());
      setErrorMessage("");
    }
    catch (std::exception & ex)
    {
      setErrorMessage(ex.what());
    }
  }
}
}