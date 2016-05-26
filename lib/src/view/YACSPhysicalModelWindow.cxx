//                                               -*- C++ -*-
/**
 *  @brief QMdiSubWindow to define a physical model
 *
 *  Copyright 2015-2016 EDF-Phimeca
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
#include "otgui/CollapsibleGroupBox.hxx"
#include "otgui/QtTools.hxx"

#include <QFileDialog>
#include <QHeaderView>
#include <QVBoxLayout>
#include <QPushButton>
#include <QFileInfo>
#include <QMessageBox>
#include <QApplication>
#include <QSettings>

using namespace OT;

namespace OTGUI {

YACSPhysicalModelWindow::YACSPhysicalModelWindow(PhysicalModelItem * item)
  : OTguiSubWindow(item)
  , physicalModel_(item->getPhysicalModel())
{
  connect(item, SIGNAL(parallelizeStatusChanged()), this, SLOT(updateParallelizeStatusWidget()));
  connect(item, SIGNAL(wantedMachineChanged()), this, SLOT(updateWantedMachineWidget()));

  QWidget * mainWidget = new QWidget;
  QVBoxLayout * mainLayout = new QVBoxLayout(mainWidget);

  // Widgets to load XML file
  QHBoxLayout * fieldsLayout = new QHBoxLayout;

  fieldsLayout->addWidget(new QLabel("Data file"));

  XMLfileNameEdit_ = new QLineEdit;
  XMLfileNameEdit_->setText(QString::fromUtf8(dynamic_cast<YACSPhysicalModel*>(&*physicalModel_.getImplementation())->getXMLFileName().c_str()));
  fieldsLayout->addWidget(XMLfileNameEdit_);

  QPushButton * selectFileButton = new QPushButton(tr("Search file"));
  connect(selectFileButton, SIGNAL(clicked()), this, SLOT(selectImportFileDialogRequested()));
  fieldsLayout->addWidget(selectFileButton);

  mainLayout->addLayout(fieldsLayout);

  // YACS scheme parameters
  CollapsibleGroupBox * YACSSchemeParametersGroupBox = new CollapsibleGroupBox;
  YACSSchemeParametersGroupBox->setTitle(tr("YACS Scheme Parameters"));
  QGridLayout * YACSSchemeParametersGroupBoxLayout = new QGridLayout(YACSSchemeParametersGroupBox);

  // parallelize status
  parallelizeStatusCheckBox_ = new QCheckBox(tr("Parallelize status"));
  parallelizeStatusCheckBox_->setChecked(dynamic_cast<YACSPhysicalModel*>(&*physicalModel_.getImplementation())->getParallelizeStatus());
  connect(parallelizeStatusCheckBox_, SIGNAL(toggled(bool)), this, SLOT(updateParallelizeStatus(bool)));
  YACSSchemeParametersGroupBoxLayout->addWidget(parallelizeStatusCheckBox_, 0, 0);

  // machines list
  QLabel * fittingMachinesLabel = new QLabel(tr("Fitting machines"));
  YACSSchemeParametersGroupBoxLayout->addWidget(fittingMachinesLabel, 1, 0);
  fittingMachinesComboBox_ = new QComboBox;
  QStringList items;
  Description fittingMachines = dynamic_cast<YACSPhysicalModel*>(&*physicalModel_.getImplementation())->getFittingMachines();
  for (UnsignedInteger i=0; i<fittingMachines.getSize(); ++i)
    items << fittingMachines[i].c_str();
  fittingMachinesComboBox_->addItems(items);
  QString wantedMachine = dynamic_cast<YACSPhysicalModel*>(&*physicalModel_.getImplementation())->getWantedMachine().c_str();
  fittingMachinesComboBox_->setCurrentIndex(fittingMachinesComboBox_->findText(wantedMachine));
  fittingMachinesComboBox_->setSizeAdjustPolicy(QComboBox::AdjustToContents);
  connect(fittingMachinesComboBox_, SIGNAL(currentIndexChanged(int)), this, SLOT(updateWantedMachine(int)));
  YACSSchemeParametersGroupBoxLayout->addWidget(fittingMachinesComboBox_, 1, 1);
  YACSSchemeParametersGroupBoxLayout->setColumnStretch(2, 1);

  YACSSchemeParametersGroupBox->setExpanded(false);

  mainLayout->addWidget(YACSSchemeParametersGroupBox);

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
  QString fileName = QFileDialog::getOpenFileName(this, tr("Data to import..."),
                     currentDir,
                     tr("Data files (*.xml);;"));

  if (!fileName.isEmpty())
  {
    QFile file(fileName);
    settings.setValue("currentDir", QFileInfo(fileName).absolutePath());

    // check
    if (!file.open(QFile::ReadOnly))
    {
      QMessageBox::warning(this, tr("Warning"),
                           tr("Cannot read file %1:\n%2").arg(fileName).arg(file.errorString()));
    }
    else
    {
      XMLfileNameEdit_->setText(fileName);
      try
      {
        dynamic_cast<YACSPhysicalModel*>(&*physicalModel_.getImplementation())->setXMLFileName(fileName.toLocal8Bit().data());

        // parallelize status
        parallelizeStatusCheckBox_->setChecked(dynamic_cast<YACSPhysicalModel*>(&*physicalModel_.getImplementation())->getParallelizeStatus());

        // fitting machines
        QStringList items;
        fittingMachinesComboBox_->clear();
        Description fittingMachines = dynamic_cast<YACSPhysicalModel*>(&*physicalModel_.getImplementation())->getFittingMachines();
        for (UnsignedInteger i=0; i<fittingMachines.getSize(); ++i)
          items << fittingMachines[i].c_str();
        fittingMachinesComboBox_->addItems(items);
        QString wantedMachine = dynamic_cast<YACSPhysicalModel*>(&*physicalModel_.getImplementation())->getWantedMachine().c_str();
        fittingMachinesComboBox_->setCurrentIndex(fittingMachinesComboBox_->findText(wantedMachine));

        setErrorMessage("");
      }
      catch (std::exception & ex)
      {
        setErrorMessage(ex.what());
      }
    }
  }
}


void YACSPhysicalModelWindow::updateParallelizeStatus(bool status)
{
  dynamic_cast<YACSPhysicalModel*>(&*physicalModel_.getImplementation())->setParallelizeStatus(status);
}


void YACSPhysicalModelWindow::updateWantedMachine(int index)
{
  dynamic_cast<YACSPhysicalModel*>(&*physicalModel_.getImplementation())->setWantedMachine(fittingMachinesComboBox_->currentText().toStdString());
}


void YACSPhysicalModelWindow::updateParallelizeStatusWidget()
{
  SignalBlocker blocker(parallelizeStatusCheckBox_);
  bool newStatus = dynamic_cast<YACSPhysicalModel*>(&*physicalModel_.getImplementation())->getParallelizeStatus();
  parallelizeStatusCheckBox_->setChecked(newStatus);
}


void YACSPhysicalModelWindow::updateWantedMachineWidget()
{
  SignalBlocker blocker(fittingMachinesComboBox_);
  QString wantedMachine = dynamic_cast<YACSPhysicalModel*>(&*physicalModel_.getImplementation())->getWantedMachine().c_str();
  fittingMachinesComboBox_->setCurrentIndex(fittingMachinesComboBox_->findText(wantedMachine));
}
}