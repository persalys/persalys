//                                               -*- C++ -*-
/**
 *  @brief QWizardPage to import sample to define designs of experiments
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
#include "otgui/ImportDataPage.hxx"

#include "otgui/FromFileDesignOfExperiment.hxx"

#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QFileDialog>
#include <QFileInfo>
#include <QSettings>
#include <QMessageBox>

using namespace OT;

namespace OTGUI {

ImportDataPage::ImportDataPage(QWidget *parent)
  : QWizardPage(parent)
  , pageValidity_(false)
{
  buildInterface();
}


void ImportDataPage::buildInterface()
{
  setWindowTitle(tr("Import table from file"));

  QGridLayout * mainGridLayout = new QGridLayout(this);

  // first row
  QHBoxLayout * hboxLayout = new QHBoxLayout;
  QLabel * label = new QLabel(tr("Data file"));
  hboxLayout->addWidget(label);

  filePathLineEdit_ = new QLineEdit;
  hboxLayout->addWidget(filePathLineEdit_);

  QPushButton * openFileButton = new QPushButton(tr("Import..."));
  connect(openFileButton, SIGNAL(clicked()), this, SLOT(openFileRequested()));
  hboxLayout->addWidget(openFileButton);

  mainGridLayout->addLayout(hboxLayout, 0, 0, 1, 3);

  // error message
  errorMessageLabel_ = new QLabel;
  errorMessageLabel_->setWordWrap(true);
  mainGridLayout->addWidget(errorMessageLabel_, 2, 0, 1, 1);

  // file preview
  QGroupBox * groupBox = new QGroupBox(tr("File Preview"));
  QGridLayout * gridLayout = new QGridLayout(groupBox);
  gridLayout->setSpacing(6);
  gridLayout->setContentsMargins(11, 11, 11, 11);

  dataPreviewTableView_ = new ExportableTableView(groupBox);
  gridLayout->addWidget(dataPreviewTableView_, 0, 0, 1, 1);

  mainGridLayout->addWidget(groupBox, 1, 0, 1, 1);
}


void ImportDataPage::openFileRequested()
{
  QSettings settings;
  QString currentDir = settings.value("currentDir").toString();
  if (currentDir.isEmpty())
    currentDir = QDir::homePath();
  QString fileName = QFileDialog::getOpenFileName(this, tr("Data to import..."),
                     currentDir,
                     tr("CSV source files (*.csv);; Text Files (*.txt)"));

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
      setData(fileName);
    }
  }
}


void ImportDataPage::setData(const QString & fileName)
{
  filePathLineEdit_->setText(fileName);
  try
  {
    errorMessageLabel_->setText("");
    setFileName(fileName);
    NumericalSample sampleFromFile = FromFileDesignOfExperiment::ImportSample(fileName.toLocal8Bit().data());
    setTable(sampleFromFile);
    pageValidity_ = true;
  }
  catch (std::exception & ex)
  {
    QString message = tr("Impossible to load the file.") + "\n";
    message = QString("%1%2%3%4").arg("<font color=red>").arg(message).arg(ex.what()).arg("</font>");
    errorMessageLabel_->setText(message);
    pageValidity_ = false;
  }
}
}