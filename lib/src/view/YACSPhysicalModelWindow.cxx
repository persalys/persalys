//                                               -*- C++ -*-
/**
 *  @brief QMdiSubWindow to define a physical model
 *
 *  Copyright 2015-2023 EDF-Phimeca
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
#include "persalys/YACSPhysicalModel.hxx"
#include "persalys/YACSPhysicalModelWindow.hxx"
#include "persalys/PhysicalModelWindowWidget.hxx"
#include "persalys/QtTools.hxx"
#include "persalys/FileTools.hxx"
#include "persalys/CheckModelButtonGroup.hxx"

#include "Py2YacsDialog.hxx"

#include <QFileDialog>
#include <QVBoxLayout>
#include <QPushButton>
#include <QFileInfo>
#include <QMessageBox>
#include <QApplication>
#include <QSettings>

using namespace OT;

namespace PERSALYS
{

YACSPhysicalModelWindow::YACSPhysicalModelWindow(PhysicalModelItem * item, QWidget * parent)
  : SubWindow(item, parent)
  , physicalModel_(item->getPhysicalModel())
  , errorMessageLabel_(0)
{
  QVBoxLayout * mainLayout = new QVBoxLayout(this);

  mainLayout->addWidget(new TitleLabel(tr("YACS model")));

  // Widgets to load XML file
  QHBoxLayout * fieldsLayout = new QHBoxLayout;

  fieldsLayout->addWidget(new QLabel("Data file"));

  QPushButton * buildSchemaButton = new QPushButton(tr("Edit"));
  connect(buildSchemaButton, SIGNAL(clicked()), this, SLOT(buildSchemaDialogRequested()));
  fieldsLayout->addWidget(buildSchemaButton);

  mainLayout->addLayout(fieldsLayout);

  // variables tables
  PhysicalModelWindowWidget * widget = new PhysicalModelWindowWidget(item);
  mainLayout->addWidget(widget);

  // error message
  errorMessageLabel_ = new TemporaryLabel;
  mainLayout->addWidget(errorMessageLabel_);

  // buttons
  CheckModelButtonGroup *buttons = new CheckModelButtonGroup;
  connect(buttons, SIGNAL(evaluateOutputsRequested()), widget, SIGNAL(evaluateOutputsRequested()));
  connect(buttons, SIGNAL(evaluateGradientRequested()), widget, SIGNAL(evaluateGradientRequested()));
  mainLayout->addWidget(buttons);


}


void YACSPhysicalModelWindow::buildSchemaDialogRequested()
{
  Py2YacsDialog yacsDialog(this);
  yacsDialog.setExportXml(false);
  YACSPhysicalModel* model = dynamic_cast<YACSPhysicalModel*>
                                     (physicalModel_.getImplementation().get());
  yacsDialog.setScriptText(model->getContent());
  if (yacsDialog.exec())
  {
    model->setContent(yacsDialog.getScriptText());
    errorMessageLabel_->reset();
  }
}
}
