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
#include "otgui/YACSPhysicalModelWindow.hxx"

#include "otgui/YACSPhysicalModel.hxx"
#include "otgui/PhysicalModelWindowWidget.hxx"
#include "otgui/QtTools.hxx"

#include "Py2YacsDialog.hxx"

#include <QFileDialog>
#include <QVBoxLayout>
#include <QPushButton>
#include <QFileInfo>
#include <QMessageBox>
#include <QApplication>
#include <QSettings>

using namespace OT;

namespace OTGUI {

YACSPhysicalModelWindow::YACSPhysicalModelWindow(PhysicalModelDefinitionItem * item, QWidget * parent)
  : OTguiSubWindow(item, parent)
  , physicalModel_(item->getPhysicalModel())
  , XMLfileNameEdit_(0)
{
  setWindowTitle(tr("YACS physical model"));

  QWidget * mainWidget = new QWidget;
  QVBoxLayout * mainLayout = new QVBoxLayout(mainWidget);

  // Widgets to load XML file
  QHBoxLayout * fieldsLayout = new QHBoxLayout;

  fieldsLayout->addWidget(new QLabel("Data file"));

  XMLfileNameEdit_ = new QLineEdit;
  XMLfileNameEdit_->setText(QString::fromUtf8(dynamic_cast<YACSPhysicalModel*>(physicalModel_.getImplementation().get())->getXMLFileName().c_str()));
  fieldsLayout->addWidget(XMLfileNameEdit_);

  QPushButton * selectFileButton = new QPushButton(tr("Search file"));
  connect(selectFileButton, SIGNAL(clicked()), this, SLOT(selectImportFileDialogRequested()));
  fieldsLayout->addWidget(selectFileButton);

  QPushButton * buildSchemaButton = new QPushButton(tr("Build YACS schema"));
  connect(buildSchemaButton, SIGNAL(clicked()), this, SLOT(buildSchemaDialogRequested()));
  fieldsLayout->addWidget(buildSchemaButton);

  mainLayout->addLayout(fieldsLayout);

  // variables tables
  PhysicalModelWindowWidget * widget = new PhysicalModelWindowWidget(item);
  connect(widget, SIGNAL(errorMessageChanged(QString)), this, SLOT(setErrorMessage(QString)));
  mainLayout->addWidget(widget);

  // error message
  errorMessageLabel_ = new QLabel;
  errorMessageLabel_->setWordWrap(true);
  mainLayout->addWidget(errorMessageLabel_);

  ////////////////
  setWidget(mainWidget);
}


void YACSPhysicalModelWindow::selectImportFileDialogRequested()
{
  QSettings settings;
  QString currentDir = settings.value("currentDir").toString();
  if (currentDir.isEmpty())
    currentDir = QDir::homePath();
  QString fileName = QFileDialog::getOpenFileName(this,
                                                  tr("Data to import..."),
                                                  currentDir,
                                                  tr("Data files (*.xml);;"));

  if (!fileName.isEmpty())
  {
    QFile file(fileName);
    settings.setValue("currentDir", QFileInfo(fileName).absolutePath());

    // check
    if (!file.open(QFile::ReadOnly))
    {
      QMessageBox::warning(this,
                           tr("Warning"),
                           tr("Cannot read file %1:\n%2").arg(fileName).arg(file.errorString()));
    }
    else
    {
      XMLfileNameEdit_->setText(fileName);
      try
      {
        dynamic_cast<YACSPhysicalModel*>(physicalModel_.getImplementation().get())->setXMLFileName(fileName.toLocal8Bit().data());

        setErrorMessage("");
      }
      catch (std::exception & ex)
      {
        setErrorMessage(ex.what());
      }
    }
  }
}


void YACSPhysicalModelWindow::buildSchemaDialogRequested()
{
  QSettings settings;
  QString currentDir = settings.value("currentDir").toString();
  if (currentDir.isEmpty())
    currentDir = QDir::homePath();

  Py2YacsDialog yacsDialog;
  if (yacsDialog.exec())
  {
    QString fileName = yacsDialog.getYacsFile();
    QFile file(fileName);
    settings.setValue("currentDir", QFileInfo(fileName).absolutePath());
    // check
    if (!file.open(QFile::ReadOnly))
    {
      QMessageBox::warning(this,
                           tr("Warning"),
                           tr("Cannot read file %1:\n%2").arg(fileName).arg(file.errorString()));
    }
    else
    {
      XMLfileNameEdit_->setText(fileName);
      try
      {
        dynamic_cast<YACSPhysicalModel*>(physicalModel_.getImplementation().get())->setXMLFileName(fileName.toLocal8Bit().data());

        setErrorMessage("");
      }
      catch (std::exception & ex)
      {
        setErrorMessage(ex.what());
      }
    }
  }
}
}
