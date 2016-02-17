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
#include "otgui/PhysicalModelWindow.hxx"

#include "otgui/AnalyticalPhysicalModel.hxx"
#include "otgui/ModelEvaluation.hxx"
#ifdef OTGUI_HAVE_YACS
# include "otgui/YACSPhysicalModel.hxx"
#endif
#include "otgui/PythonPhysicalModel.hxx"
#include "otgui/CodeDelegate.hxx"

#include <QFileDialog>
#include <QComboBox>
#include <QHeaderView>

using namespace OT;

namespace OTGUI {

PhysicalModelWindow::PhysicalModelWindow(PhysicalModelItem * item)
  : OTguiSubWindow(item)
  , physicalModel_(item->getPhysicalModel())
  , inputTableModel_(0)
  , outputTableModel_(0)
  , codeModel_(0)
{
  connect(this, SIGNAL(physicalModelChanged(const PhysicalModel&)), item, SLOT(updatePhysicalModel(const PhysicalModel&)));
  connect(item, SIGNAL(outputChanged()), this, SLOT(updateOutputTableModel()));
  connect(item, SIGNAL(inputChanged()), this, SLOT(updateInputTableModel()));
  connect(item, SIGNAL(codeChanged()), this, SLOT(updateCodeModel()));
  buildInterface();
}


void PhysicalModelWindow::buildInterface()
{
  QWidget * mainWidget = new QWidget;
  QVBoxLayout * mainLayout = new QVBoxLayout(mainWidget);

  // Choose method to define Inputs and Outputs
  QGroupBox * methodBox = new QGroupBox(tr("Physical model type"));
  QHBoxLayout * methodLayout = new QHBoxLayout(methodBox);

  QComboBox * comboBox = new QComboBox;
  QStringList items = QStringList()<<tr("Analytical")<<tr("Python");
#ifdef OTGUI_HAVE_YACS
  items<<tr("XML");
#endif
  comboBox->addItems(items);

  if (physicalModel_.getImplementation()->getClassName() == "AnalyticalPhysicalModel")
    comboBox->setCurrentIndex(0);
  else if(physicalModel_.getImplementation()->getClassName() == "PythonPhysicalModel")
    comboBox->setCurrentIndex(1);
#ifdef OTGUI_HAVE_YACS
  else if(physicalModel_.getImplementation()->getClassName() == "YACSPhysicalModel")
    comboBox->setCurrentIndex(2);
#endif

  connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(methodChanged(int)));
  methodLayout->addWidget(comboBox);
  methodLayout->addStretch();

  mainLayout->addWidget(methodBox);

  pythonDefinitionBox_ = new QGroupBox(tr(""));
  QVBoxLayout *pythonLayout = new QVBoxLayout;

  codeView_ = new QTableView;
  codeView_->setEditTriggers(QTableView::AllEditTriggers);
  codeView_->horizontalHeader()->setStretchLastSection(true);
  codeView_->verticalHeader()->setStretchLastSection(true);
  codeView_->horizontalHeader()->hide();
  codeView_->verticalHeader()->hide();
  codeView_->setItemDelegate(new CodeDelegate);
  pythonLayout->addWidget(codeView_);
  pythonDefinitionBox_->setLayout(pythonLayout);
  mainLayout->addWidget(pythonDefinitionBox_);

#ifdef OTGUI_HAVE_YACS
  // Widgets to load XML file
  loadXMLFileBox_ = new QGroupBox(tr(""));
  QGridLayout * fieldsLayout = new QGridLayout(loadXMLFileBox_);

  QLabel * labelDataFile = new QLabel("Data file");
  fieldsLayout->addWidget(labelDataFile, 0, 0);

  XMLfileNameEdit_ = new QLineEdit;
  if(physicalModel_.getImplementation()->getClassName() == "YACSPhysicalModel")
    XMLfileNameEdit_->setText(dynamic_cast<YACSPhysicalModel*>(&*physicalModel_.getImplementation())->getXMLFileName().c_str());
  fieldsLayout->addWidget(XMLfileNameEdit_, 0, 1);

  QPushButton * selectFileButton = new QPushButton(tr("Search file"));
  connect(selectFileButton, SIGNAL(clicked()), this, SLOT(selectImportFileDialogRequested()));
  fieldsLayout->addWidget(selectFileButton, 0, 2);

  XMLErrorMessage_ = new QLabel;
  XMLErrorMessage_->setWordWrap(true);
  fieldsLayout->addWidget(XMLErrorMessage_, 1, 0, 1, 2, Qt::AlignTop);

  mainLayout->addWidget(loadXMLFileBox_);
#endif

  // Define Inputs
  QGroupBox * inputsBox = new QGroupBox(tr("Inputs"));
  QVBoxLayout * inputsLayout = new QVBoxLayout(inputsBox);

  inputTableView_ = new QTableView;
  inputTableView_->setEditTriggers(QTableView::AllEditTriggers);
  inputsLayout->addWidget(inputTableView_);

  addInputLineButton_ = new QPushButton(QIcon(":/images/list-add.png"), tr("Add"));
  addInputLineButton_->setToolTip(tr("Add an input"));
  connect(addInputLineButton_, SIGNAL(clicked(bool)), this, SLOT(addInputLine()));

  removeInputLineButton_ = new QPushButton(QIcon(":/images/list-remove.png"), tr("Remove"));
  removeInputLineButton_->setToolTip(tr("Remove the selected input"));
  connect(removeInputLineButton_, SIGNAL(clicked(bool)), this, SLOT(removeInputLine()));

  QHBoxLayout * buttonsLayout = new QHBoxLayout;
  buttonsLayout->addStretch();
  buttonsLayout->addWidget(addInputLineButton_);
  buttonsLayout->addWidget(removeInputLineButton_);
  inputsLayout->addLayout(buttonsLayout);

  mainLayout->addWidget(inputsBox);

  // Define Outputs
  QGroupBox * outputsBox = new QGroupBox(tr("Outputs"));
  QVBoxLayout * outputsLayout = new QVBoxLayout(outputsBox);

  outputTableView_ = new QTableView;
  outputTableView_->setEditTriggers(QTableView::AllEditTriggers);
  outputsLayout->addWidget(outputTableView_);

  addOutputLineButton_ = new QPushButton(QIcon(":/images/list-add.png"), tr("Add"));
  addOutputLineButton_->setToolTip(tr("Add an output"));
  connect(addOutputLineButton_, SIGNAL(clicked(bool)), this, SLOT(addOutputLine()));

  removeOutputLineButton_ = new QPushButton(QIcon(":/images/list-remove.png"), tr("Remove"));
  removeOutputLineButton_->setToolTip(tr("Remove the selected output"));
  connect(removeOutputLineButton_, SIGNAL(clicked(bool)), this, SLOT(removeOutputLine()));

  evaluateOutputsButton_ = new QPushButton(tr("Evaluate"));
  evaluateOutputsButton_->setToolTip(tr("Evaluate the value of the outputs"));
  connect(evaluateOutputsButton_, SIGNAL(clicked(bool)), this, SLOT(evaluateOutputs()));

  buttonsLayout = new QHBoxLayout;
  buttonsLayout->addStretch();
  buttonsLayout->addWidget(addOutputLineButton_);
  buttonsLayout->addWidget(removeOutputLineButton_);
  buttonsLayout->addWidget(evaluateOutputsButton_);
  outputsLayout->addLayout(buttonsLayout);

  mainLayout->addWidget(outputsBox);

  errorMessageLabel_ = new QLabel;
  errorMessageLabel_->setWordWrap(true);
  mainLayout->addWidget(errorMessageLabel_);

  updateMethodWidgets(comboBox->currentIndex());
  ////////////////
  setWidget(mainWidget);
}


void PhysicalModelWindow::selectImportFileDialogRequested()
{
  QString xmlPath;
  QString fileName = QFileDialog::getOpenFileName(this, tr("Data to import..."),
                     xmlPath,
                     tr("Data files (*.xml);;"));

  if (!fileName.isEmpty())
  {
    XMLfileNameEdit_->setText(fileName);
    loadXML();
  }
}


void PhysicalModelWindow::loadXML()
{
#ifdef OTGUI_HAVE_YACS
  try
  {
    physicalModel_ = YACSPhysicalModel(physicalModel_.getName(), XMLfileNameEdit_->text().toStdString());
    emit physicalModelChanged(physicalModel_);
    updateInputTableModel();
    updateOutputTableModel();
    XMLErrorMessage_->setText("");
    setErrorMessage("");
  }
  catch (Exception & ex)
  {
    XMLErrorMessage_->setText(QString("%1%2%3").arg("<font color=red>").arg(ex.what()).arg("</font>"));
    setErrorMessage(ex.what());
  }
#endif
}


void PhysicalModelWindow::updateCodeModel()
{
  if (codeModel_)
    delete codeModel_;
  codeModel_ = new CodeModel(physicalModel_);
  codeView_->setModel(codeModel_);
}


void PhysicalModelWindow::updateInputTableModel()
{
  if (inputTableModel_)
    delete inputTableModel_;
  inputTableModel_ = new InputTableModel(physicalModel_);
  inputTableView_->setModel(inputTableModel_);
}


void PhysicalModelWindow::updateOutputTableModel()
{
  if (outputTableModel_)
    delete outputTableModel_;
  outputTableModel_ = new OutputTableModel(physicalModel_);
  outputTableView_->setModel(outputTableModel_);
}


void PhysicalModelWindow::addInputLine()
{
  inputTableModel_->addLine();
}


void PhysicalModelWindow::addOutputLine()
{
  outputTableModel_->addLine();
}


void PhysicalModelWindow::updateMethodWidgets(int method)
{
  addInputLineButton_->setVisible(method!=2);
  removeInputLineButton_->setVisible(method!=2);
  addOutputLineButton_->setVisible(method!=2);
  removeOutputLineButton_->setVisible(method!=2);

#ifdef OTGUI_HAVE_YACS
  loadXMLFileBox_->setVisible(method==2);
#endif
  pythonDefinitionBox_->setVisible(method==1);

  updateInputTableModel();
  updateOutputTableModel();
  updateCodeModel();

  outputTableView_->setColumnHidden(2, method!=0);
}


void PhysicalModelWindow::methodChanged(int method)
{
  setErrorMessage("");
  switch(method)
  {
    case 0:
    {
      physicalModel_ = AnalyticalPhysicalModel(physicalModel_.getName());
      break;
    }
    case 1:
    {
      physicalModel_ = PythonPhysicalModel(physicalModel_.getName());
      break;
    }
#ifdef OTGUI_HAVE_YACS
    case 2:
    {
      physicalModel_ = PhysicalModel(physicalModel_.getName());
      break;
    }
#endif
    default:
      throw std::exception();
  }
  emit physicalModelChanged(physicalModel_);
  updateMethodWidgets(method);
}


void PhysicalModelWindow::removeInputLine()
{
  if (inputTableView_->selectionModel()->hasSelection())
  {
    QModelIndex index = inputTableView_->selectionModel()->currentIndex();
    inputTableModel_->removeLine(index);
    int lastRow = inputTableModel_->rowCount()-1;

    if (lastRow+1)
      inputTableView_->selectRow(lastRow);
  }
}


void PhysicalModelWindow::removeOutputLine()
{
  if (outputTableView_->selectionModel()->hasSelection())
    outputTableModel_->removeLine(outputTableView_->selectionModel()->currentIndex());
}


void PhysicalModelWindow::evaluateOutputs()
{
  try
  {
    ModelEvaluation eval("anEval", physicalModel_);
    eval.run();
    NumericalSample outputSample(eval.getResult().getOutputSample());
    for (UnsignedInteger i = 0; i < outputSample.getDimension(); ++ i)
      physicalModel_.setOutputValue(physicalModel_.getOutputNames()[i], outputSample[0][i]);

    setErrorMessage("");
  }
  catch (Exception & ex)
  {
    setErrorMessage(ex.what());
  }
}
}